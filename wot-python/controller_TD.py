def get_td(HTTP_ip_address):
	return{
		"@context": "https://www.w3.org/2019/wot/td/v1",
	    "id": "ch.unisg:ics:wot-fx:uarm-controller",
	    "title": "uArm-Controller",
	    "securityDefinitions": { "nosec_sc": {"scheme": "nosec"}
	    },
	    "security": ["nosec_sc"],
	    "properties": {
	    	"version":{
	    		"title":"Version",
	    		"description":"Returns the firmware version installed on the controller.",
	    		"type":"String",
				"forms": [
					{
					"href": "http://{}/uarm-controller/properties/version".format(HTTP_ip_address),
					"op": ["readproperty"],
					"contentType":"application/json"
					}
				]
	    	},
			"ram":{
	    		"title":"RAM usage",
	    		"description":"Returns the RAM space usage of the controller",
	    		"type":"String",
				"forms": [
					{"href": "http://{}/uarm-controller/properties/ram".format(HTTP_ip_address),
					"op": ["readproperty"],
					"contentType":"application/json"
					}
				]
	    	}

	    },
	
	    "actions":{
			"toggle-led":{
				"title":"Toggle Red LED",
				"description":"Toggle the red LED",
				"forms":[{
					"href":"http://{}/uarm-controller/actions/toggle".format(HTTP_ip_address),
					"contentType":"application/json",
					"op":"invokeaction",
					"htv:methodName":"POST"
				}],
				"idempotent": False,
				"safe": False
			}
		}
			
}
