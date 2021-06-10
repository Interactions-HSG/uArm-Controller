#! /usr/bin/env python3

import os
import sys
import time
import threading
import json
import logging
import line_protocol_pb2

from flask import Flask, request, jsonify, json, abort
from jsonschema import Draft6Validator
from controller import *

TD = None

app = Flask(__name__)

""" ---------- uArm Controller ---------- """
# fct returns TD
@app.route('/uarm-controller')
def controller_thing_description():
    return json.dumps(TD[0]), {'Content-Type':'application/json'}

# fct toggle red led on controller
@app.route('/uarm-controller/actions/toggle', methods=["POST"])
def toggle_red_led():
    if red_led_profile.pin_state:
        """ send action to turn led on"""
        red_led_profile.write_digital(line_protocol_pb2.LOW)
        response = "Turned red LED on."
    else:
        """ send action to turn led off"""
        red_led_profile.write_digital(line_protocol_pb2.HIGH)
        response = "Turned red LED off."
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

""" ---------- uArm ---------- """
# fct returns TD
@app.route('/uarm-1')
def uarm_thing_description():
    return json.dumps(TD[1]), {'Content-Type':'application/json'}

# fct send G-code to uArm
@app.route('/uarm-1/actions/sendcommand', methods=["POST"])
def uarm_send_command():
    if request.is_json:
        schema = TD[1]["actions"]["sendcommand"]["input"]
        valid_input = Draft6Validator(schema).is_valid(request.json)
        
        if valid_input:
            data = request.get_data()
            json_data = json.loads(data)
            cmd = json_data['command']
            # default True
            if 'blocking' in json_data:
                blocking = json_data['blocking']
                print("test")
            else:
                blocking = True
            
            """ check if uArm is available """
            if uarm_profile.profile_state == ProfileState.WAITING and not(blocking):
                """ uArm not yet available"""               
                uarm_profile.send_command(cmd, not(blocking))               
                return "G-code sent: in waiting mode."
            else:
                """ send G-code"""
                uarm_profile.send_command(cmd, not(blocking)) 
                return "G-code sent: started execution."
        else:
            abort(400)
    else:
        abort(415)

""" ---------- Colorsensor ---------- """
# fct returns TD
@app.route('/colorsensor')
def colorsensor_thing_description():
    return json.dumps(TD[2]), {'Content-Type':'application/json'}

#fct returns rgb values
@app.route('/colorsensor/properties/values', methods=["GET"])
def colorsensor_get_values():

    colorsensor_profile.action_profile()
    rgb = { 
            "r": colorsensor_profile.r,
            "g": colorsensor_profile.g,
            "b": colorsensor_profile.b}
    return (rgb)

#fct returns estimated color
@app.route('/colorsensor/properties/estimatedcolor', methods=["GET"])
def colorsensor_get_color():

    colorsensor_profile.action_profile()
    color = colorsensor_profile.estimated_color
    return (color)


""" ---------- Helper Functions ---------- """
def push_HTTP_TD(TD_HTTP):
    global TD
    TD = TD_HTTP
