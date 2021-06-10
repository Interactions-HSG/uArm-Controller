"""
Main for WoT-Python implementation of the uArm-Controller.

Build on structure from: https://github.com/tum-esi/wot-sys/tree/master/Devices/python-Uarm
"""

from controller import *
from HTTP import *
from controller_TD import get_td
import time
import requests
import _thread

time.sleep(10)

# controller port
#controller_port = "COM4"

# initialize controller: register profiles
#init_controller(controller_port)

#IP address
HTTP_ip = "192.168.8.102"
HTTP_port = "8080"
HTTP_ip_address = HTTP_ip + ":" + HTTP_port
TD_Directory_Address = "http://" + HTTP_ip_address

TD_Controller = get_td( HTTP_ip_address)

push_HTTP_TD(TD_Controller)

def submit_td(ip_addr,td):
	print("Uploading TD to directory ...")
	while True:
		try:
			r = requests.post("{}/api/td".format(ip_addr),json=td)
			r.close()
			print("Got response: ", r.status_code)
			if 200 <= r.status_code <= 299:
				print("TD uploaded")
				return
		except Exception as e:
			print(e)
			print("TD could not be uploaded. Will try again in 15 Seconds ...")
			time.sleep(15)


_thread.start_new_thread(submit_td, (TD_Directory_Address,TD_Controller))


#starts HTTP flask server
app.run(host=HTTP_ip, port=8080)
