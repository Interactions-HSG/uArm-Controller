"""
Main for WoT-Python implementation of the uArm-Controller.

Build on structure from: https://github.com/tum-esi/wot-sys/tree/master/Devices/python-Uarm
"""
import time
import requests
import _thread
import json
import re

from controller import *
from HTTP import *


time.sleep(10)


#IP address
HTTP_ip = "192.168.8.102"
HTTP_port = "8080"
HTTP_ip_address = HTTP_ip + ":" + HTTP_port
TD_Directory_Address = "http://" + HTTP_ip_address

# get TD for uArm-Controller and replace placeholders
with open('TD/controller.json', 'r') as f:
	json_raw = f.read()
	json_replaced = json_raw.replace('<IP_ADD>', HTTP_ip_address)
	TD_Controller = json.loads(json_replaced)

# get TD for uArm and replace placeholders
with open('TD/uarm.json', 'r') as f:
	json_raw = f.read()
	json_replaced = json_raw.replace('<IP_ADD>', HTTP_ip_address)
	TD_Uarm = json.loads(json_replaced)

# get TD for colorsensor and replace placeholders
with open('TD/colorsensor.json', 'r') as f:
	json_raw = f.read()
	json_replaced = json_raw.replace('<IP_ADD>', HTTP_ip_address)
	TD_Colorsensor = json.loads(json_replaced)

TD = [TD_Controller, TD_Uarm, TD_Colorsensor]
push_HTTP_TD(TD)
      
#starts HTTP flask server
app.run(host=HTTP_ip, port=8080)
