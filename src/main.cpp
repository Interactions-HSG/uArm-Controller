/**************************************************************************/
/*!
    @file     main.cpp
    @author   Jonas Brütsch

    Main routine for the uFactory Controller firmware.

    TODO: Brief description of structure

*/
/**************************************************************************/

#include <main.h>

/*========================================================================*/
/*                  PRIVATE DEFINITIONS                                   */
/*========================================================================*/
/* Macros */
#define BAUDRATE 115200
#define TERMINATOR 0

/* Variables */
bool last_req_decode_success = false;
// status to indicate setup phase => no feedback after device initialization
bool setup_flag = false;

/* Protobuf streams */
pb_istream_s pb_in;
pb_ostream_s pb_out;

/* Function prototypes */
/**
    @brief  Handles incoming  Request messages
*/
void request_handler();

/**
    @brief  Handles incoming Action messages
    @param  action: Action message
*/
void action_handler(Action action);

/**
    @brief  Handles incoming Registration messages
    @param  registration: Registration message
*/
void registration_handler(Registration registration);



/*========================================================================*/
/*                  INITIALIZATION                                        */
/*========================================================================*/
void setup(void)
{ 
  // indicate setup phase: no feedback should be sent on re-initialization
  setup_flag = true;

  /* init the serial i/f for the MES */
  Serial.begin(BAUDRATE);
  pb_in = as_pb_istream(Serial);
  pb_out = as_pb_ostream(Serial);

  // TODO: initialize SD card manager
  // TODO: load registrations from SD card => re-initialize stored profiles

  // stop setup phase
  setup_flag = false;
  delay(1000);
}

/*========================================================================*/
/*                  MAIN LOOP                                             */
/*========================================================================*/
void loop(void)
{ 
  // process incoming message
  request_handler();
  delay(1000);
}

/*========================================================================*/
/*                  FUNCTION DEFINITIONS                                  */
/*========================================================================*/
/**************************************************************************/
/*
    Request Handler: handles incoming request messages
*/ 
void request_handler()
{
  // process the incoming packet if the buffer is not empty
  if (Serial.available() > 0)
  { 
    // current request message
    Request req; 

    
    // FIXME: works but returns not true => check why
    last_req_decode_success = pb_decode(&pb_in, Request_fields, &req);
    // check if action or registration
    if (req.which_request_type == Request_action_tag){
      action_handler(req.request_type.action);
      send_msg(req.which_request_type);
    }
    else if (req.which_request_type == Request_registration_tag){
      registration_handler(req.request_type.registration);
      send_msg(req.which_request_type);
    }
    else {//TODO: implement better error feedback messages
      // FIXME: does not work every time!  
      send_msg(404);
      send_msg(req.which_request_type);
    }
  }
}

/**************************************************************************/
/*
    Action Handler: handles incoming actions
*/ 
void action_handler(Action action)
{ 
  // TODO: check if profile_id is registered => if not: send ERROR msg
  // use corresponding driver function
  switch(action.which_driver){
    case Action_a_digital_generic_tag:
      // call action function of digital_generic driver
      run_digital_generic(action.profile_id, action.driver.a_digital_generic);
      // send confirmation
      send_msg(action.profile_id); //, "led possibly registered");
      break;
    case Action_a_uart_ttl_generic_tag:
      //call driver function
      break;
    default:
      // ERROR: no driver functions definded for specified registration
      break;
  } 
}

/**************************************************************************/
/*
    Registration Handler: handles incoming registrations
*/ 
void registration_handler(Registration registration)
{
  // TODO: implement different activations (Polling, event-driven) 
  

  // clear old profile, if already registered
  delete_profile(registration.profile_id);
  
  // use corresponding driver function
  switch(registration.which_driver){
    case Registration_r_digital_generic_tag:
      // call initialization function of digital_generic driver
      init_digital_generic( registration.profile_id, registration.driver.r_digital_generic);
      // send confirmation if not in setup phase
      if(!setup_flag)
        send_msg(registration.profile_id); //, "led possibly registered");
      break;
    case Registration_r_uart_ttl_generic_tag:
      //call driver function
      break;
    default:
      // ERROR: no driver functions definded for specified registration
      break;
  }

  // use profile manager to save new registration on registered_profiles + on SD card
  store_profile(registration);


}

/**************************************************************************/
/*
    Function used to send feedback to the gateway 
    TODO:
    => needs to be updated to be able to handle string fields
    => should be used for all feedbacks (Polling feedback + Event feedback)
    => better name for function
*/ 
bool send_msg(uint32_t profile_id) //, String msg)
{
  Feedback feedback = {};
  feedback.message.funcs.d
  feedback.profile_id = profile_id;
  //feedback.message = msg;
  bool res = pb_encode(&pb_out, Feedback_fields, &feedback);
  Serial.write(TERMINATOR);
  return res;
}
