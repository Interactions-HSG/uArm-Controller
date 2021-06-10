# -*- coding: utf-8 -*-
"""
Controller module for uArm Controller.

This module provides a serial handler to communicate with uArm Controller Arduino MEGA 2560.
<Event#> is a decimal value defined in ufactory.event.Event
"""
import sys
import threading
import time
import math
import os


import logging
import serial
import serial.threaded
from enum import Enum

import line_protocol_pb2

""" ---------- Variables ---------- """

# baudrate for UART
BAUDRATE = 115200


"""" ---------- Classes for profiles ---------- """


class ProfileState(Enum):
    """Enum to define possible profile states.

    Attributes:
        UNREG: Unregistered (profile needs first to be registered on the MCU)
        IDLE: Profile is registered and available
        BLOCKING: Profile is waiting for ACK or DATA => Blocking state for entire MCU
        WAITING: Profile is waiting for response/data => Non-Blocking state for other profiles
    """

    UNREG = 1
    IDLE = 2
    BLOCKING = 3
    WAITING = 4


class ProfileManager(list):
    """ Class to manage registered/created profiles. """

    def get_profile(self, profile_id):
        for profile in self:
            if profile.profile_id == profile_id:
                return profile


class Profile:
    """Parent class for profiles.

    Attributes:
        profile_state: Current state of the profile
        profile_id: The unique identifier for the profile.
    """

    # state of profile: if profile is successfully registered on MCU
    profile_state = ProfileState.UNREG

    def __init__(self, profile_id):
        self.profile_id = profile_id
        # save profile in list and delete profile with same id
        profile = profiles.get_profile(profile_id)
        if profile is not None:
            profiles.remove(profile)
        profiles.append(self)

    def register_wait(self):
        """ Function to wait until profile is registered """
        # TODO: implement timeout?
        while self.profile_state == ProfileState.UNREG:
            time.sleep(0.1)

    def action_wait(self):
        """ Function to wait until profile is registered """
        # TODO: implement timeout?
        while self.profile_state == ProfileState.BLOCKING:
            time.sleep(0.1)


class DigitalGeneric(Profile):
    """ Profile for digital generic driver """

    # state of pin: True -> HIGH, False -> LOW
    pin_state = False

    def __init__(self, profile_id, pin, mode):
        """The constructor creates an instance of a digital generic profile.

        Args:
            profile_id ([uint8]): unique profile id
            pin ([int]): number of digital pin
            mode ([Enum]): digital mode (INPUT, OUTPUT, INPUT_PULLUP)
        """
        self.pin = pin
        self.mode = mode
        super().__init__(profile_id)

    def register_profile(self):
        """ register new profile on MCU """
        req = line_protocol_pb2.Request()
        # pylint: disable=no-member
        req.registration.profile_id = self.profile_id
        req.registration.r_digital_generic.pin = self.pin
        req.registration.r_digital_generic.mode = self.mode
        controller.send(req.SerializeToString())
        logging.info(" Registration sent for Profile: %i", self.profile_id)
        super().register_wait()

    def write_digital(self, output):
        """ Write digital pin to HIGH/LOW """
        req = line_protocol_pb2.Request()
        # pylint: disable=no-member
        req.action.profile_id = self.profile_id
        req.action.a_digital_generic.output = output
        controller.send(req.SerializeToString())
        if output == line_protocol_pb2.HIGH:
            logging.info(
                " Digital pin action: HIGH sent (Profile: %i)", self.profile_id
            )
            self.pin_state = True  # IDEA: do this on receiving ack
        elif output == line_protocol_pb2.LOW:
            logging.info(
                " Digital pin action: LOW sent (Profile: %i)", self.profile_id)
            self.pin_state = False  # IDEA: do this on receiving ack
        self.profile_state = ProfileState.BLOCKING
        super().action_wait()

    def read_digital(self):
        """ Read digital pin """
        req = line_protocol_pb2.Request()
        # pylint: disable=no-member
        req.action.profile_id = self.profile_id
        req.action.a_digital_generic.event_triggered = False
        controller.send(req.SerializeToString())
        logging.info(
            " Digital pin action: Read pin (Profile: %i)", self.profile_id)
        self.profile_state = ProfileState.BLOCKING
        super().action_wait()
        return self.pin_state

    def read_event_digital(self, trigger_value):
        """Start event listening for digital pin.

        Args:
            trigger_value (HIGH/LOW): used to set value on which event will be triggered
        """
        req = line_protocol_pb2.Request()
        # pylint: disable=no-member
        req.action.profile_id = self.profile_id
        req.action.a_digital_generic.output = trigger_value
        req.action.a_digital_generic.event_triggered = True
        controller.send(req.SerializeToString())
        logging.info(
            " Digital pin action: Start event listening (Profile: %i)", self.profile_id
        )
        self.profile_state = ProfileState.BLOCKING
        super().action_wait()

    def data_handler(self, data):
        """Handles incoming data from actions or events.

        Args:
            data (byte): 1:LOW or 2:HIGH (added one to avoid empty byte)
        """
        self.pin_state = ord(data) - 1
        logging.info(
            ">> Digital DATA: received state: %i (Profile: %i)",
            self.pin_state,
            self.profile_id,
        )


class UartTTLGeneric(Profile):
    def __init__(self, profile_id, port, baudrate):
        """The constructor creates an instance of a UART-TTL-generic profile.

        Args:
            profile_id ([type]): [description]
            port ([type]): [description]
            baudrate ([type]): [description]
        """
        self.port = port
        self.baudrate = baudrate
        super().__init__(profile_id)

    def create_cmd_list(self, cmd_list):
        self.cmd_list = cmd_list
        self.cmd_list_iterator = iter(self.cmd_list)

    def register_profile(self):
        """ register new profile on MCU """
        req = line_protocol_pb2.Request()
        # pylint: disable=no-member
        req.registration.profile_id = self.profile_id
        req.registration.r_uart_ttl_generic.port = self.port
        req.registration.r_uart_ttl_generic.baudrate = self.baudrate
        controller.send(req.SerializeToString())
        logging.info(" UART: Registration sent (Profile: %i)", self.profile_id)
        super().register_wait()

    def send_next(self):
        """ send next command in command list """
        command = next(self.cmd_list_iterator, False)
        if command:
            self.send_command(command[0], command[1])

    def send_command(self, command, event):
        """ Send command to UART2/3 """
        req = line_protocol_pb2.Request()
        # pylint: disable=no-member
        req.action.profile_id = self.profile_id
        req.action.a_uart_ttl_generic.command = command
        req.action.a_uart_ttl_generic.event_triggered = event
        controller.send(req.SerializeToString())
        logging.info(" UART: Command sent (Profile: %i)", self.profile_id)
        self.profile_state = ProfileState.BLOCKING
        super().action_wait()

    def data_handler(self, data):
        """Handles incoming data from actions or events.

        Args:
            data (char[]): char array containing the uArm feedback message
        """
        logging.info(">> UART TTL Response: %s", data.decode("utf-8"))


class ColorSensor(Profile):
    """ Profile for color_sensor driver

        TODO: implement event handling => event on specific color range
    """

    def __init__(self, profile_id):
        """The constructor creates an instance of a color_sensor profile.

        Args:
            profile_id ([uint8]): unique profile id
        """
        self.r = 0
        self.g = 0
        self.b = 0
        self.estimated_color = "Unknown"
        super().__init__(profile_id)

    def register_profile(self):
        """ Register new profile on MCU """
        req = line_protocol_pb2.Request()
        # pylint: disable=no-member
        req.registration.profile_id = self.profile_id
        req.registration.r_color_sensor.address = 0  # currently not used
        controller.send(req.SerializeToString())
        logging.info(" Registration sent for Profile: %i", self.profile_id)
        super().register_wait()

    def action_profile(self):
        """ Action function for color_sensor profiles """
        req = line_protocol_pb2.Request()
        # pylint: disable=no-member
        req.action.profile_id = self.profile_id
        req.action.a_color_sensor.event_triggered = False  # currently not supported
        controller.send(req.SerializeToString())
        logging.info(
            " Read request for color sensor sent (Profile: %i)", self.profile_id
        )
        self.profile_state = ProfileState.BLOCKING
        super().action_wait()

    def _estimate_color(self):
        _red = [165, 57, 52, "Red"]
        _yellow = [255, 255, 110, "Yellow"]
        _green = [49, 90, 65, "Green"]
        _wood = [201, 186, 126, "Wood"]
        _colors = [_red, _yellow, _green, _wood]
        _distances = [0, 0, 0, 0]
        for i, color in enumerate(_colors):
            _distances[i] = math.sqrt(
                (color[0]-self.r)**2 + (color[1]-self.g)**2 + (color[2]-self.b)**2)
        min_index = _distances.index(min(_distances))
        self.estimated_color = _colors[min_index][3]

    def data_handler(self, data):
        """Handles incoming data from actions or events.

        Args:
            data (byte[3]): RGB value with one byte for each color
        """
        self.r = data[0]
        self.g = data[1]
        self.b = data[2]
        logging.info(
            ">> Color sensor DATA: R: %i, G: %i, B: %i (Profile: %i)",
            self.r,
            self.g,
            self.b,
            self.profile_id,
        )
        self._estimate_color()
        logging.info("Estimated color: %s", self.estimated_color)


class UltrasonicSensor(Profile):
    """ Profile for ultrasonic_sensor driver """

    def __init__(self, profile_id, pin):
        """The constructor creates an instance of a ultrasonic_sensor profile.

        Args:
            profile_id ([uint8]): unique profile id
            pin ([uint32]): pin number for SIG: receiver/transmitter
        """
        self.pin = pin
        super().__init__(profile_id)

    def register_profile(self):
        """ Register new profile on MCU """
        req = line_protocol_pb2.Request()
        # pylint: disable=no-member
        req.registration.profile_id = self.profile_id
        req.registration.r_ultrasonic_sensor.pin = self.pin
        controller.send(req.SerializeToString())
        logging.info(
            " Registration sent for Profile: %i  <=======", self.profile_id)
        super().register_wait()

    def action_profile(self):
        """ Action function for ultrasonic_sensor profiles """
        req = line_protocol_pb2.Request()
        # pylint: disable=no-member
        req.action.profile_id = self.profile_id
        req.action.a_ultrasonic_sensor.event_triggered = False  # not supported
        controller.send(req.SerializeToString())
        logging.info(
            "Ultrasonic sensor: sent action (Profile: %i)  <=======", self.profile_id)
        self.profile_state = ProfileState.BLOCKING
        super().action_wait()

    def data_handler(self, data):
        """Handles incoming data from actions or events.

        Args:
            data (uint16): int containing distance in cm
        """
        # first bit is used as flag to avoid null bytes
        distance = (data[0] & 0b01111111) + (data[1] & 0b01111111)*128
        logging.info(
            ">> Utrasonic sensor DATA: distance: %i cm (Profile: %i) <=======",
            distance,
            self.profile_id,
        )


class StepMotor(Profile):
    """ Profile for step_motor driver 

        NOT TESTED YET!
    """

    def __init__(self, profile_id):
        """The constructor creates an instance of a step_motor profile.

        Args:
            profile_id ([uint8]): unique profile id
        """
        super().__init__(profile_id)

    def register_profile(self):
        """ Register new profile on MCU """
        req = line_protocol_pb2.Request()
        # pylint: disable=no-member
        req.registration.profile_id = self.profile_id
        req.registration.r_step_motor.SetInParent()  # needed if message is empty
        controller.send(req.SerializeToString())
        logging.info(" Registration sent for Profile: %i", self.profile_id)
        super().register_wait()

    def set_speed(self, direction, time_min_val, wait):
        """ Action function for step_motor profiles to set the motor speed """
        req = line_protocol_pb2.Request()
        # pylint: disable=no-member
        req.action.profile_id = self.profile_id
        req.action.a_step_motor.direction = direction
        req.action.a_step_motor.time_min_val = time_min_val
        req.action.a_step_motor.wait = wait
        controller.send(req.SerializeToString())
        self.profile_state = ProfileState.BLOCKING
        super().action_wait()

    def set_steps(self, steps, time_min_val, wait):
        """ Action function for step_motor profiles to set the motor steps """
        req = line_protocol_pb2.Request()
        # pylint: disable=no-member
        req.action.profile_id = self.profile_id
        req.action.a_step_motor.steps = steps
        req.action.a_step_motor.time_min_val = time_min_val
        req.action.a_step_motor.wait = wait
        controller.send(req.SerializeToString())
        self.profile_state = ProfileState.BLOCKING
        super().action_wait()

    def data_handler(self, data):
        """Handles incoming data from actions or events.

        Args:
            data ([type]): TODO: has to be defined
        """
        # TODO: implement data handling
        pass


class McuDriver(Profile):
    """ Profile for mcu_driver driver """

    def __init__(self, profile_id):
        """The constructor creates an instance of a mcu_driver profile.

        Args:
            profile_id ([uint8]): unique profile id
        """
        super().__init__(profile_id)

    def register_profile(self):
        """ Register new profile on MCU """
        req = line_protocol_pb2.Request()
        # pylint: disable=no-member
        req.registration.profile_id = self.profile_id
        req.registration.r_mcu_driver.SetInParent()
        controller.send(req.SerializeToString())
        logging.info(" Registration sent for Profile: %i", self.profile_id)
        super().register_wait()

    def get_version(self):
        """ Action function to get current firmware version """
        req = line_protocol_pb2.Request()
        # pylint: disable=no-member
        req.action.profile_id = self.profile_id
        req.action.a_mcu_driver.mcu_action = line_protocol_pb2.VERSION
        self.curr_request = line_protocol_pb2.VERSION
        controller.send(req.SerializeToString())
        self.profile_state = ProfileState.BLOCKING
        super().action_wait()

    def get_ram(self):
        """ Action function to get free RAM space """
        req = line_protocol_pb2.Request()
        # pylint: disable=no-member
        req.action.profile_id = self.profile_id
        req.action.a_mcu_driver.mcu_action = line_protocol_pb2.RAM
        self.curr_request = line_protocol_pb2.RAM
        controller.send(req.SerializeToString())
        self.profile_state = ProfileState.BLOCKING
        super().action_wait()

    def data_handler(self, data):
        """Handles incoming data from actions or events.

        Args:
            data ([type]): TODO: has to be defined
        """
        if self.curr_request == line_protocol_pb2.VERSION:
            logging.info(">> MCU firmware version: %s", data.decode("utf-8"))
        elif self.curr_request == line_protocol_pb2.RAM:
            # first bit is used as flag to avoid null bytes
            ram_space = 8192
            used_space = (data[0] & 0b01111111) + (data[1] & 0b01111111)*(2**8)
            #used_space = 8192 - free_space
            percentage = round((used_space/ram_space)*100, 2)
            logging.info(">> MCU RAM: [%s%s] %i%% (used %i bytes from %i bytes)",
                         '='*int(round(percentage/10)),
                         ' '*(10-int(round(percentage/10))),
                         percentage, used_space, ram_space)
# ADI-PY-Profile: Label for automatic driver initialization (Do not move!)


"""" ---------- Classes for protobuf message handling ---------- """


class ControllerPacketHandler(serial.threaded.Packetizer):
    """Callback for received packet"""

    def handle_packet(self, packet):
        response = line_protocol_pb2.Response()
        response.ParseFromString(packet)
        # pylint: disable=no-member

        # print entire msg for debugging
        # print(response.__str__())
        if response.code == line_protocol_pb2.DEBUG:
            logging.debug(">> %s", response.payload.decode("utf-8"))

        elif response.code == line_protocol_pb2.ERROR:
            logging.error(
                ">> Profile: %i %s",
                response.profile_id,
                response.payload.decode("utf-8")
            )
        elif response.code == line_protocol_pb2.ACK:
            profile = profiles.get_profile(response.profile_id)
            profile.profile_state = ProfileState.WAITING
            logging.info(">> ACK for profile: %s received",
                         response.profile_id)

        elif response.code == line_protocol_pb2.DATA:
            profile = profiles.get_profile(response.profile_id)
            if profile.profile_state == ProfileState.UNREG:
                """ DATA for registration """
                profile.profile_state = ProfileState.IDLE
                logging.info(
                    ">> Registration DATA for profile: %s received",
                    response.profile_id)
            elif profile.profile_state == ProfileState.BLOCKING:
                profile.profile_state = ProfileState.IDLE
                if not len(response.payload) == 0:
                    profile.data_handler(response.payload)
                else:
                    logging.info(
                        ">> Empty action DATA for profile: %s received", response.profile_id
                    )
            elif profile.profile_state == ProfileState.WAITING:
                profile.profile_state = ProfileState.IDLE
                if not len(response.payload) == 0:
                    profile.data_handler(response.payload)
                else:
                    logging.info(
                        ">> Empty event DATA for profile: %s received", response.profile_id
                    )


class Controller(serial.threaded.ReaderThread):
    """"""

    def __init__(self, serial_device, event_handler):
        """
        The constructor creates a serial instance for given dev/url with a preset option
        :param serial_device: device or url for the serial interface
        :param event_handler: event handler of class ControllerHandler
        """
        serial_instance = serial.serial_for_url(
            serial_device, baudrate=115200, timeout=1
        )
        super(Controller, self).__init__(serial_instance, event_handler)

    def send(self, protobuf):
        """ send the given protobuf message """
        self.serial.write(protobuf)
        self.serial.write(b"\0")
        return


"""" ---------- Profile creations ---------- """

# create list of all profiles
profiles = ProfileManager()

# define used Profile IDs (div: green will change to blue)
mcu_driver_id = 0
red_LED_profile_id = 1
div_LED_profile_id = 2
button_A_profile_id = 5
uArm1_profile_id = 10
uArm2_profile_id = 11
color_sensor_id = 20
ultrasonic_sensor_id = 21
tube_sensor_id = 22

# create profile for MCU
McuDriver(mcu_driver_id)
# create profile for red LED
#DigitalGeneric(red_LED_profile_id, 2, line_protocol_pb2.OUTPUT)
# create profile for green LED
#DigitalGeneric(div_LED_profile_id, 3, line_protocol_pb2.OUTPUT)
# create profile for button A (event triggered)
#DigitalGeneric(button_A_profile_id, 47, line_protocol_pb2.INPUT_PULLUP)
# create profile for UART2-TTL: uArm1
UartTTLGeneric(uArm1_profile_id, line_protocol_pb2.UART2, BAUDRATE)
profiles.get_profile(uArm1_profile_id).create_cmd_list(
    [("G0 X180 Y0 Z160 F50\n", True),
     ("M2210 F2000 T200\n", False),
     ("M2210 F1000 T300\n", False),
     ("G0 X180 Y50 Z100 F50\n", True),
     ("M2210 F1000 T200\n", False),
     ("P2220\n", False),
     ("G0 X180 Y100 Z180 F50\n", True),
     ("M2210 F1000 T200\n", False),
     ("P2220\n", False),
     ("M2231 V1\n", True),
     ("G0 X180 Y50 Z100 F50\n", True),
     ("M2210 F1000 T200\n", False),
     ("P2220\n", False),
     ("M2231 V0\n", True),
     ("M2210 F1000 T300\n", False),
     ("M2210 F2000 T200\n", False),
     ])
# create profile for UART3-TTL: uArm2
# UartTTLGeneric(uArm2_profile_id, line_protocol_pb2.UART3, BAUDRATE)

# create profile for color sensor
ColorSensor(color_sensor_id)
# create profile for ultrasonic sensor
#UltrasonicSensor(ultrasonic_sensor_id, 23)
# create profile for tube sensor
DigitalGeneric(tube_sensor_id, 25, line_protocol_pb2.INPUT_PULLUP)


def subroutine_test():
    """ Subroutine to test combination of sensors/actuators """

    # DEMO:
    # create profile for ultrasonic sensor
    ultrasonic_profile = UltrasonicSensor(50, 23)
    ultrasonic_profile.register_profile()

    # get needed profiles
    uArm_profile = profiles.get_profile(uArm1_profile_id)
    color_profile = profiles.get_profile(color_sensor_id)
    tube_profile = profiles.get_profile(tube_sensor_id)

    # reset uArm
    uArm_profile.send_command("G0 X180 Y0 Z160 F30\n", False)
    uArm_profile.send_command("M2210 F2000 T200\n", False)
    uArm_profile.send_command("M2210 F1000 T300\n", False)

    # counter used for number of colored cubes
    num_color_cubes = 0

    # while cube available on ramp
    while True:
        # DEMO:
        # measure hight
        ultrasonic_profile.action_profile

        if not tube_profile.read_digital():
            # robot to ramp
            uArm_profile.send_command("G0 X84 Y-160 Z70 F100\n", False)
            uArm_profile.send_command("G0 X84 Y-160 Z50 F20\n", False)
            # pump on
            uArm_profile.send_command("M2231 V1\n", False)
            time.sleep(0.5)
            uArm_profile.send_command("G0 X84 Y-160 Z90 F20\n", False)

            # robot go to color sensor
            uArm_profile.send_command("G0 X141 Y-76 Z70 F50\n", False)
            uArm_profile.send_command("G0 X141 Y-76 Z46 F20\n", False)

            # Color sensor: measure color
            time.sleep(1)
            color_profile.action_profile()
            uArm_profile.send_command("G0 X141 Y-76 Z55 F20\n", False)

            if color_profile.estimated_color == "Wood":
                uArm_profile.send_command("G0 X104 Y160 Z55 F50\n", False)
                uArm_profile.send_command("G0 X104 Y160 Z28 F20\n", False)
                # pump off
                uArm_profile.send_command("M2231 V0\n", False)
                uArm_profile.send_command("G0 X104 Y160 Z55 F50\n", False)
            else:
                num_color_cubes += 1
                end_destination = num_color_cubes*30
                uArm_profile.send_command(
                    "G0 X178 Y160 Z{} F50\n".format(end_destination + 10), False)
                uArm_profile.send_command(
                    "G0 X178 Y160 Z{} F5\n".format(end_destination), False)
                # pump off
                uArm_profile.send_command("M2231 V0\n", False)
                uArm_profile.send_command(
                    "G0 X178 Y160 Z{} F5\n".format(end_destination + 10), False)

            # go to start position
            uArm_profile.send_command("G0 X180 Y0 Z160 F100\n", False)

        else:
            uArm_profile.send_command("M2210 F1000 T300\n", False)
            uArm_profile.send_command("M2210 F2000 T200\n", False)
            break


"""" ---------- Main ---------- """

if __name__ == "__main__":
    logging.basicConfig(format="%(levelname)s:%(message)s",
                        level=logging.DEBUG)

    controller = Controller(sys.argv[1], ControllerPacketHandler)
    controller.start()
    time.sleep(3)

    """ register all profiles"""
    for profile in profiles:
        profile.register_profile()

    counter = 0
    simple_tests = False

    """ get current driver version + current RAM usage """
    profile = profiles.get_profile(mcu_driver_id)
    if profile is not None:
        if profile.profile_state == ProfileState.IDLE:
            profile.get_version()
            profile.get_ram()

    while simple_tests:

        """ Test for tube Sensor """
        # profile = profiles.get_profile(tube_sensor_id)
        # if profile is not None:
        #     if profile.profile_state == ProfileState.IDLE:
        #         profile.read_digital()

        """ Test for Ultrasonic Sensor driver """
        profile = profiles.get_profile(ultrasonic_sensor_id)
        if profile is not None:
            if profile.profile_state == ProfileState.IDLE:
                profile.action_profile()
        time.sleep(1)
        # """ Test for Color Sensor driver """
        # profile = profiles.get_profile(color_sensor_id)
        # if profile is not None:
        #     if profile.profile_state == ProfileState.IDLE:
        #         profile.action_profile()

        # """ Test for event handling: call event for button A """
        # profile = profiles.get_profile(button_A_profile_id)
        # if profile is not None:
        #     if profile.profile_state == ProfileState.IDLE:
        #         profile.read_event_digital(line_protocol_pb2.LOW)

        # """ Test: updating profile """
        # if counter == 4:
        #     # create profile for blue LED (same ID as green!)
        #     profile = DigitalGeneric(
        #         div_LED_profile_id, 5, line_protocol_pb2.OUTPUT)
        #     profile.register_profile()
        # elif counter == 9:
        #     # create profile for green LED (same ID as blue => overwritten!)
        #     profile = DigitalGeneric(
        #         div_LED_profile_id, 3, line_protocol_pb2.OUTPUT)
        #     profile.register_profile()
        # counter = (counter + 1) % 10

        # """ Test: toggle green/blue LED """
        # profile = profiles.get_profile(div_LED_profile_id)
        # if profile is not None:
        #     if profile.profile_state == ProfileState.IDLE:
        #         if profile.pin_state:
        #             """ send action to turn led off"""
        #             profile.write_digital(line_protocol_pb2.LOW)
        #         else:
        #             """ send action to turn led on"""
        #             profile.write_digital(line_protocol_pb2.HIGH)

        # """ send gcode command to uArm1 """
        # profile = profiles.get_profile(uArm1_profile_id)
        # if profile is not None:
        #     if profile.profile_state == ProfileState.IDLE:
        #         # profile.send_command(" P2220\n", False)
        #         profile.send_next()

        # """ Test: toggle red LED """
        # profile = profiles.get_profile(red_LED_profile_id)
        # if profile is not None:
        #     if profile.profile_state == ProfileState.IDLE:
        #         if profile.pin_state:
        #             """ send action to turn led off"""
        #             profile.write_digital(line_protocol_pb2.LOW)
        #         else:
        #             """ send action to turn led on"""
        #             profile.write_digital(line_protocol_pb2.HIGH)

    try:
        subroutine_test()
    except KeyboardInterrupt:
        uArm_profile = profiles.get_profile(uArm1_profile_id)
        uArm_profile.send_command("M2231 V0\n", False)
        # TODO: implement proper interrupt handling
        #uArm_profile.send_command("G0 X180 Y0 Z160 F50\n", False)
        print('Interrupted')
        try:
            sys.exit(0)
        except SystemExit:
            os._exit(0)
