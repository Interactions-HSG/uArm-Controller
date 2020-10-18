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

/* Variables */
// status to indicate setup phase => no feedback after device initialization
bool setup_flag = false;

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

  // initialize protobuf message communication 
  protobuf_init();

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

    // decode the received protobuf message
    protobuf_decode(&req);
    //send_feedback(200, "message received");
    
    // check if action or registration
    if (req.which_request_type == Request_action_tag){
      action_handler(req.request_type.action);
    }
    else if (req.which_request_type == Request_registration_tag){
      registration_handler(req.request_type.registration);
    }
    else 
      // ERROR: request type of msg is incorrect
      send_feedback(404, "ERROR: request type of msg is incorrect");
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
      break;

    case Action_a_uart_ttl_generic_tag:
      //call action function of generic UART TTL driver
      run_uart_ttl_generic(action.profile_id, action.driver.a_uart_ttl_generic);
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
  // clear old profile, if already registered
  delete_profile(registration.profile_id);
  
  // boolean to check whether initialization was successfull or not
  bool reg_success = false;

  /* initializing with corresponding driver function */ 
  switch(registration.which_driver){

    case Registration_r_digital_generic_tag:
      // call initialization function
      reg_success = init_digital_generic( registration.profile_id, registration.driver.r_digital_generic);    
      break;

    case Registration_r_uart_ttl_generic_tag:
      //call initialization function
      reg_success = init_uart_ttl_generic( registration.profile_id, registration.driver.r_uart_ttl_generic);
      break;

    default:
      // ERROR: no driver functions definded for specified registration
      send_feedback(registration.profile_id, "ERROR: no driver functions definded for specified registration");
      break;
  }

  // TODO: save registration in registered profiles list if successful
  
  /* send feedback if not in re-initialization phase */
  // send confirmation if registration successfull
  if(!setup_flag && reg_success){
    send_feedback(registration.profile_id, "Registration was successful");
    // use profile manager to save new registration on registered_profiles + on SD card
    store_profile(registration);
  }
  // send ERROR if registration failed
  else if(!setup_flag && !reg_success)
    send_feedback(registration.profile_id, "ERROR: Registration failed");

}

