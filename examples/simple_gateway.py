# -*- coding: utf-8 -*-
"""
Controller module for uArm Controller.

This module provides a serial handler to communicate with uArm Controller Arduino MEGA 2560.
<Event#> is a decimal value defined in ufactory.event.Event
"""
import sys
import threading
import time


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
        WAIT: Profile is waiting for ACK => Blocking state for entire MCU
        BUSY: Profile is waiting for response/data => Non-Blocking state for other profiles
    """

    UNREG = 1
    IDLE = 2
    WAIT = 3
    BUSY = 4


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

    def register_wait(self):
        """ Function to wait until profile is registered """
        # TODO: implement timeout?
        while self.profile_state == ProfileState.UNREG:
            time.sleep(0.1)

    def action_wait(self):
        """ Function to wait until profile is registered """
        # TODO: implement timeout?
        while self.profile_state == ProfileState.WAIT:
            time.sleep(0.1)
            logging.debug("...waiting...")


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
        """ write digital pin to HIGH/LOW """
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
            logging.info(" Digital pin action: LOW sent (Profile: %i)", self.profile_id)
            self.pin_state = False  # IDEA: do this on receiving ack
        self.profile_state = ProfileState.WAIT
        super().action_wait()


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

    def send_command(self, command):
        """ Send command to UART2/3 """
        req = line_protocol_pb2.Request()
        # pylint: disable=no-member
        req.action.profile_id = self.profile_id
        req.action.a_uart_ttl_generic.command = command
        controller.send(req.SerializeToString())
        logging.info(" UART: Command sent (Profile: %i)", self.profile_id)
        self.profile_state = ProfileState.WAIT
        super().action_wait()


"""" ---------- Classes for protobuf message handling ---------- """


class ControllerPacketHandler(serial.threaded.Packetizer):
    """Callback for received packet"""

    def handle_packet(self, packet):
        feedback = line_protocol_pb2.Feedback()
        feedback.ParseFromString(packet)
        # pylint: disable=no-member

        # print entire msg for debugging
        # print(feedback.__str__())
        if feedback.code == line_protocol_pb2.DEBUG:
            logging.info(">> DEBUG: %s", repr(feedback.message))
        elif feedback.code == line_protocol_pb2.ERROR:
            logging.error(
                ">> Profile: %i %s", feedback.profile_id, repr(feedback.message)
            )
        elif feedback.code == line_protocol_pb2.ACK:
            for profile in profiles:
                if profile.profile_id == feedback.profile_id:
                    profile.profile_state = ProfileState.BUSY
                    logging.info(">> ACK for profile: %s received", feedback.profile_id)
                    break
        elif feedback.code == line_protocol_pb2.DONE:
            logging.debug("DONE for %i", feedback.profile_id)
            for profile in profiles:
                if profile.profile_id == feedback.profile_id:
                    if profile.profile_state == ProfileState.UNREG:
                        """ DONE for registration """
                        profile.profile_state = ProfileState.IDLE
                        logging.info(
                            ">> Registration DONE for profile: %s received",
                            feedback.profile_id,
                        )
                    elif profile.profile_state == ProfileState.WAIT:
                        """ DONE for last action """
                        profile.profile_state = ProfileState.IDLE
                        # TODO: handle DONE for last action
                        logging.info(
                            ">> Activation ACK for profile: %s received",
                            feedback.profile_id,
                        )
                    break
        elif feedback.code == line_protocol_pb2.DATA:
            logging.info(">> DATA for profile: %s received", feedback.profile_id)
            # TODO: handle incoming data messages


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

# list of all profiles
profiles = []

# create profile for red LED
red_LED_profile = DigitalGeneric(1, 2, line_protocol_pb2.OUTPUT)
profiles.append(red_LED_profile)
# create profile for green LED
green_LED_profile = DigitalGeneric(2, 3, line_protocol_pb2.OUTPUT)
profiles.append(green_LED_profile)
# create profile for blue LED (same ID as green!)
blue_LED_profile = DigitalGeneric(2, 5, line_protocol_pb2.OUTPUT)
# create profile for UART2-TTL: uArm1
uArm1_profile = UartTTLGeneric(10, line_protocol_pb2.UART2, BAUDRATE)
profiles.append(uArm1_profile)
# create profile for UART3-TTL: uArm2
uArm2_profile = UartTTLGeneric(11, line_protocol_pb2.UART3, BAUDRATE)
profiles.append(uArm2_profile)


"""" ---------- Main ---------- """

if __name__ == "__main__":
    logging.basicConfig(format="%(levelname)s:%(message)s", level=logging.DEBUG)

    controller = Controller(sys.argv[1], ControllerPacketHandler)
    controller.start()
    time.sleep(3)

    """ register all profiles"""
    for profile in profiles:
        profile.register_profile()

    counter = 0

    while True:
        """ Test updating profile """
        # if counter == 2:
        #     blue_LED_profile.register_profile()
        # elif counter == 9:
        #     green_LED_profile.register_profile()
        # counter = (counter + 1) % 10
        # TODO: not working => profiles[] must be updated!

        """ toggle red LED """
        if red_LED_profile.profile_state == ProfileState.IDLE:
            if red_LED_profile.pin_state:
                """ send action to turn led off"""
                red_LED_profile.write_digital(line_protocol_pb2.LOW)
            else:
                """ send action to turn led on"""
                red_LED_profile.write_digital(line_protocol_pb2.HIGH)

        """ toggle green LED """
        if green_LED_profile.profile_state == ProfileState.IDLE:
            if green_LED_profile.pin_state:
                """ send action to turn led off"""
                green_LED_profile.write_digital(line_protocol_pb2.LOW)
            else:
                """ send action to turn led on"""
                green_LED_profile.write_digital(line_protocol_pb2.HIGH)

        """ send gcode command to uArm1 """
        if uArm1_profile.profile_state == ProfileState.IDLE:
            uArm1_profile.send_command("G0 X180 Y0 Z160 F500\n")

        # """ send gcode command to uArm2 """
        # if uArm2_profile.is_registered:
        #    uArm2_profile.send_command("G0 X100 Y100 Z120 F500\n")
        # time.sleep(1)
