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

import line_protocol_pb2

""" ---------- Variables ---------- """

# baudrate for UART
baudrate = 115200


"""" ---------- Classes for profiles ---------- """

class Profile:
    """ Parent class for profiles """
    # state of profile: if profile is successfully registered on MCU
    is_registered = False

    def __init__(self, profile_id, activation):
        self.profile_id = profile_id
        self.activation = activation
    """ CRUD operations """
    def create_profile(self):
        pass       
    def read_profile(self): # not used yet
        pass
    def update_profile(self): # not used yet
        pass
    def delete_profile(self): # not used yet
        pass


class DigitalGeneric(Profile):
    """ Profile for digital generic driver """
    # state of pin: True -> HIGH, False -> LOW
    pin_state = False

    def __init__(self, profile_id, activation, pin, mode):
        """
        The constructor creates an instance of a digital generic profile
        :param profile_id: unique profile id
        :param activation: TODO:
        :param pin: number of digital pin
        :param mode: digital mode (INPUT, OUTPUT, INPUT_PULLUP)
        :type profile_id: 8-bit number
        :type activation: TODO:
        :type pin: int
        :type mode: TODO:
        """
        self.pin = pin
        self.mode = mode
        super().__init__(profile_id, activation)
    
    def create_profile(self):
        """ register new profile on MCU """
        req = line_protocol_pb2.Request()
        req.registration.profile_id = self.profile_id
        req.registration.activation = self.activation
        req.registration.r_digital_generic.pin = self.pin
        req.registration.r_digital_generic.mode = self.mode
        controller.send(req.SerializeToString())
        logging.info(" Registration sent for Profile: %i", self.profile_id)
    
    def write_digital(self, output):
        """ write digital pin to HIGH/LOW """
        req = line_protocol_pb2.Request()
        req.action.profile_id = self.profile_id
        req.action.a_digital_generic.output = output
        controller.send(req.SerializeToString())
        if output == line_protocol_pb2.HIGH:
            logging.info(" Digital pin action: HIGH sent (Profile: %i)", self.profile_id)
            self.pin_state = True    # IDEA: do this on receiving ack
        elif output == line_protocol_pb2.LOW:
            logging.info(" Digital pin action: LOW sent (Profile: %i)", self.profile_id)
            self.pin_state = False   # IDEA: do this on receiving ack


class UartTTLGeneric(Profile):
    def __init__(self, profile_id, activation, port, baudrate):
        """
        The constructor creates an instance of a digital generic profile
        :param profile_id: unique profile id
        :param activation: TODO:
        :param port: UART2 or UART3
        :param baudrate: digital mode (INPUT, OUTPUT, INPUT_PULLUP)
        :type profile_id: 8-bit number
        :type activation: TODO:
        :type port: int
        :type baudrate: int
        """
        self.port = port
        self.baudrate = baudrate
        super().__init__(profile_id, activation)
    
    def create_profile(self):
        """ register new profile on MCU """
        req = line_protocol_pb2.Request()
        req.registration.profile_id = self.profile_id
        req.registration.activation = self.activation
        req.registration.r_uart_ttl_generic.port = self.port
        req.registration.r_uart_ttl_generic.baudrate = self.baudrate
        controller.send(req.SerializeToString())
        logging.info(" UART: Registration sent (Profile: %i)", self.profile_id)
    
    def send_command(self, command):
        """ Send command to UART2/3 """
        req = line_protocol_pb2.Request()
        req.action.profile_id = self.profile_id
        req.action.a_uart_ttl_generic.command = command
        controller.send(req.SerializeToString())
        logging.info(" UART: Command sent (Profile: %i)", self.profile_id)


"""" ---------- Classes for protobuf message handling ---------- """

class ControllerPacketHandler(serial.threaded.Packetizer):
    """Callback for received packet"""
    def handle_packet(self, packet):
        feedback = line_protocol_pb2.Feedback()
        feedback.ParseFromString(packet)
        logging.info(">> Profile_ID: %d, Message: %s", feedback.profile_id, repr(feedback.message))
        # print entire msg for debugging
        #print(feedback.__str__())
        
        """ if message is registration acknowledgement => set corresponding is_registrated field """
        # TODO: use feedback code field instead of message => needs to be implemented first
        if feedback.message == "Registration was successful":
            for profile in profiles:
                if profile.profile_id == feedback.profile_id:
                    # set registration status to true
                    profile.is_registered = True
                    break
            else:
                logging.error("received profile does not exist")


class Controller(serial.threaded.ReaderThread):
    """
    """
    def __init__(self, serial_device, event_handler):
        """
        The constructor creates a serial instance for given dev/url with a preset option
        :param serial_device: device or url for the serial interface
        :param event_handler: event handler of class ControllerHandler
        """
        serial_instance = serial.serial_for_url(serial_device, baudrate=115200, timeout=1)
        super(Controller, self).__init__(serial_instance, event_handler)

    def send(self, protobuf):
        """ send the given protobuf message """
        self.serial.write(protobuf)
        self.serial.write(b'\0') 
        return


"""" ---------- Profile creations ---------- """

# list of all profiles
profiles = []

# create profile for red LED
red_LED_profile = DigitalGeneric( 1, line_protocol_pb2.POLLING, 2, line_protocol_pb2.OUTPUT)
profiles.append(red_LED_profile)
# create profile for green LED
green_LED_profile = DigitalGeneric( 2, line_protocol_pb2.POLLING, 3, line_protocol_pb2.OUTPUT)
profiles.append(green_LED_profile)
# create profile for UART2-TTL: uArm1 TODO: change activation
uArm1_profile = UartTTLGeneric( 10, line_protocol_pb2.COMPLEX, line_protocol_pb2.UART2, baudrate)
profiles.append(uArm1_profile)
# create profile for UART3-TTL: uArm2 TODO: change activation
#uArm2_profile = UartTTLGeneric( 11, line_protocol_pb2.COMPLEX, line_protocol_pb2.UART3, baudrate)
#profiles.append(uArm2_profile)


"""" ---------- Main ---------- """

if __name__ == '__main__':
    logging.basicConfig(format='%(levelname)s:%(message)s', level=logging.DEBUG)

    controller = Controller(sys.argv[1], ControllerPacketHandler)
    controller.start()
    time.sleep(3)
    
    
    """ register all profiles"""
    for profile in profiles:
        profile.create_profile()
        while(not profile.is_registered):
            time.sleep(0.1)   

    while(True):

        """ toggle red LED """
        if red_LED_profile.is_registered and red_LED_profile.pin_state:
            """ send action to turn led off"""
            red_LED_profile.write_digital(line_protocol_pb2.LOW)
        elif red_LED_profile.is_registered and not red_LED_profile.pin_state:
            """ send action to turn led on"""
            red_LED_profile.write_digital(line_protocol_pb2.HIGH)
        time.sleep(1)   #TODO: implement better blocking mechanism (similar to registration blocking)

        """ toggle green LED """
        if green_LED_profile.is_registered and green_LED_profile.pin_state:
            """ send action to turn led off"""
            green_LED_profile.write_digital(line_protocol_pb2.LOW)
        elif green_LED_profile.is_registered and not green_LED_profile.pin_state:
            """ send action to turn led on"""
            green_LED_profile.write_digital(line_protocol_pb2.HIGH)
        time.sleep(1)


        """ send gcode command to uArm1 """
        if uArm1_profile.is_registered:
            uArm1_profile.send_command("G0 X180 Y0 Z160 F500\n")
        time.sleep(1)

        #""" send gcode command to uArm2 """
        #if uArm2_profile.is_registered:
        #    uArm2_profile.send_command("G0 X100 Y100 Z120 F500\n")
        #time.sleep(1)