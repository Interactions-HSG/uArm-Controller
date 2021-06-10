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


class McuDriver(Profile):
    """ Profile for mcu_driver driver """

    def __init__(self, profile_id):
        """The constructor creates an instance of a mcu_driver profile.

        Args:
            profile_id ([uint8]): unique profile id
        """
        self.version = None
        self.ram = None
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
            self.version = data.decode("utf-8")
            logging.info(">> MCU firmware version: %s", self.version)
        elif self.curr_request == line_protocol_pb2.RAM:
            # first bit is used as flag to avoid null bytes
            ram_space = 8192
            used_space = (data[0] & 0b01111111) + (data[1] & 0b01111111)*(2**8)
            percentage = round((used_space/ram_space)*100, 2)
            logging.info(">> MCU RAM: [%s%s] %i%% (used %i bytes from %i bytes)",
                         '='*int(round(percentage/10)),
                         ' '*(10-int(round(percentage/10))),
                         percentage, used_space, ram_space)
            self.ram = [percentage, used_space, ram_space]

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

"""" ---------- Controller Initialization ---------- """
# create profiles
mcu_profile = McuDriver(0)
red_led_profile = DigitalGeneric(10, 2, line_protocol_pb2.OUTPUT)
    
controller = Controller("COM4", ControllerPacketHandler)
controller.start()
time.sleep(3)

""" register all profiles"""
mcu_profile.register_profile()
red_led_profile.register_profile()
