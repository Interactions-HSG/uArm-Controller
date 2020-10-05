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

import heartbeat_pb2


class ControllerPacketHandler(serial.threaded.Packetizer):
    """Callback for received packet"""
    def handle_packet(self, packet):
        payload = heartbeat_pb2.Input()
        payload.ParseFromString(packet)
        # pylint: disable=no-member
        logging.info(">> pin: %d, volume: %d", payload.pin, payload.volume)
        interval = self.transport.interval
        gap = self.transport.gap
        if payload.pin == 33: # UP
            interval += gap
            if interval > 5:
                interval = 5
        elif payload.pin == 41: # DOWN
            interval -= gap
            if interval < 0.01:
                interval = 0.01
        elif payload.pin == 32: # LEFT
            gap /= 10
            if gap < 0.01:
                gap = 0.01
        elif payload.pin == 37: # RIGHT
            gap *= 10
            if gap > 1:
                gap = 1
        if self.transport.interval != interval or self.transport.gap != gap:
            self.transport.interval = interval
            self.transport.gap = gap
            logging.info("interval: %f, gap: %f", self.transport.interval, self.transport.gap)

class Controller(serial.threaded.ReaderThread):
    """
    """
    interval = 0.1
    gap = 0.1
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

    while(True):
        controller.interval = 1
        for i in range(1000):
            req = heartbeat_pb2.Request()
            # pylint: disable=no-member
            req.id = i
            logging.info("<< id: %d", req.id)
            controller.send(req.SerializeToString())
            time.sleep(controller.interval)
