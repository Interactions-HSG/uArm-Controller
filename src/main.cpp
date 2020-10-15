/**************************************************************************/
/*!
    @file     main.cpp
    @author   Jonas Brütsch

    Main routine for the uFactory Controller firmware.

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
    if (req.which_request_type == Request_action_tag)
      action_handler(req.request_type.action);
    else if (req.which_request_type == Request_registration_tag)
      registration_handler(req.request_type.registration);
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
  // TODO: implement different activations (Polling, event-driven) 
  // 1. check if profile_id already registered in registrations-list:
  //     yes => call profile_manager::delete(profile_id) (to delete old registered profile + clear all ports in config-list)
  // 2. TODO: use driver manager to initialize profile => driver_manager.cpp with driver_registration() driver_action()
  
  // use corresponding driver function
  switch(registration.which_driver){

    case Registration_r_digital_generic_tag:
      // call initialization function
      init_digital_generic( registration.profile_id, registration.driver.r_digital_generic);
      // send confirmation if not in setup phase
      if(!setup_flag)
        send_feedback(registration.profile_id, "LED is inizialized");
      break;

    case Registration_r_uart_ttl_generic_tag:
      //call initialization function
      init_uart_ttl_generic( registration.profile_id, registration.driver.r_uart_ttl_generic);
      // send confirmation if not in setup phase
      if(!setup_flag)
        send_feedback(registration.profile_id, "UART TTL is initialized");
      break;

    default:
      // ERROR: no driver functions definded for specified registration
      send_feedback(404, "ERROR: no driver functions definded for specified registration");
      break;
  }

  // 3. check if interfaces already registered in config-file => if yes, remove old from config-file
  // 4. use config manager to save new entry in config-file (1. in struct, 2. save in SD card)


}
