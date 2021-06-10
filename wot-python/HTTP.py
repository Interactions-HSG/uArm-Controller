#! /usr/bin/env python3

import os
import sys
import time
import threading
import json
import controller # needed?
import line_protocol_pb2

from flask import Flask, request, jsonify, json, abort
from jsonschema import Draft6Validator
from controller import *

TD = 0

app = Flask(__name__)

# fct returns TD TODO: check if works with /uarm-controller?
@app.route('/uarm-controller')
def thing_description():
    return json.dumps(TD), {'Content-Type':'application/json'}

# fct toggle red led on controller
@app.route('/uarm-controller/actions/toggle', methods=["POST"])
def toggle_red_led():
    if red_led_profile.pin_state:
        """ send action to turn led off"""
        red_led_profile.write_digital(line_protocol_pb2.LOW)
        response = "Turned red LED off."
    else:
        """ send action to turn led on"""
        red_led_profile.write_digital(line_protocol_pb2.HIGH)
        response = "Turned red LED on."
    return (response)

#fct returns firmware version
@app.route('/uarm-controller/properties/version', methods=["GET"])
def get_version():

    mcu_profile.get_version()
    version = mcu_profile.version
    return (version)

#fct returns available RAM space
@app.route('/uarm-controller/properties/ram', methods=["GET"])
def get_ram():

    mcu_profile.get_ram()
    ram = { 
            "ram space": mcu_profile.ram[2],
            "used space": mcu_profile.ram[1],
            "free space": mcu_profile.ram[2] - mcu_profile.ram[1],
            "percentage (used)": mcu_profile.ram[0]}
    return (ram)


def push_HTTP_TD(TD_HTTP):
    global TD
    TD = TD_HTTP
