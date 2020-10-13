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

#import heartbeat_pb2
import line_protocol_pb2

# variables
""" registration status of R LED """
r_LED_reg_status = False
r_LED_on = False
r_LED_profile_id = 333

class ControllerPacketHandler(serial.threaded.Packetizer):
    """Callback for received packet"""
    def handle_packet(self, packet):
        feedback = line_protocol_pb2.Feedback()
        feedback.ParseFromString(packet)
        #logging.info(">> asset_id: %d, message: %s", feedback.asset_id, feedback.message)
        logging.info(">> received feedback with profile_id: %d", feedback.profile_id)
        # if correct message received => led should be initialized
        global r_LED_reg_status
        global r_LED_on
        if(feedback.profile_id == r_LED_profile_id):
            r_LED_reg_status = True
        # turn led off if already on
        if(r_LED_reg_status and r_LED_on):
            """ send action to turn led off"""
            req_R = line_protocol_pb2.Request()
            req_R.action.profile_id = r_LED_profile_id
            req_R.action.a_digital_generic.pin = 2
            req_R.action.a_digital_generic.output = line_protocol_pb2.LOW
            controller.send(req_R.SerializeToString())
            logging.info("Red LED action: LOW sent")
            r_LED_on = False
        elif(r_LED_reg_status and not(r_LED_on)):
            """ send action to turn led off"""
            req_R = line_protocol_pb2.Request()
            req_R.action.profile_id = r_LED_profile_id
            req_R.action.a_digital_generic.pin = 2
            req_R.action.a_digital_generic.output = line_protocol_pb2.HIGH
            controller.send(req_R.SerializeToString())
            logging.info("Red LED action: HIGH sent")
            r_LED_on = True
            

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

if __name__ == '__main__':
    logging.basicConfig(format='%(levelname)s:%(message)s', level=logging.DEBUG)

    controller = Controller(sys.argv[1], ControllerPacketHandler)
    controller.start()
    time.sleep(3)

    """ register R LED with generic digital request"""
    req_R = line_protocol_pb2.Request()
    req_R.registration.profile_id = r_LED_profile_id
    req_R.registration.activation = line_protocol_pb2.POLLING
    req_R.registration.r_digital_generic.pin = 2
    req_R.registration.r_digital_generic.mode = line_protocol_pb2.OUTPUT
    controller.send(req_R.SerializeToString())
    logging.info("Red LED: registration sent")

    while(True):
   
        """ toggle red LED if already registered"""
        if(not(r_LED_reg_status)):
            logging.info("red LED not registered yet..")
        time.sleep(1)
