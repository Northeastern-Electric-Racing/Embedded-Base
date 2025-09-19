
#include "threadX_utils.h"
#include "u_threads.h"
#include "u_threads.h"
#include "u_general.h"

uint8_t _create_thread(TX_BYTE_POOL *byte_pool, thread_t *thread) {
     /* Create Threads */
    //CATCH_ERROR(_create_thread(byte_pool, &_default_thread), 0);            // Create Default thread.
    
    /*
    CATCH_ERROR(_create_thread(byte_pool, &_can_thread), U_SUCCESS);                
    CATCH_ERROR(_create_thread(byte_pool, &_faults_thread), U_SUCCESS);             
    CATCH_ERROR(_create_thread(byte_pool, &_statemachine_thread), U_SUCCESS);    
    CATCH_ERROR(_create_thread(byte_pool, &_nonfunctional_data_thread), U_SUCCESS); 
    CATCH_ERROR(_create_thread(byte_pool, &_tsms_thread), U_SUCCESS);               
    CATCH_ERROR(_create_thread(byte_pool, &_control_thread), U_SUCCESS);            
    CATCH_ERROR(_create_thread(byte_pool, &_pedals_thread), U_SUCCESS);  
    */        
    // add more threads here if need

    DEBUG_PRINTLN("Ran threads_init().");
    return U_SUCCESS;
}

uint8_t threads_init(TX_BYTE_POOL *byte_pool);

uint8_t threads_init(TX_BYTE_POOL *byte_pool) {

    /* Create Threads */
    //CATCH_ERROR(_create_thread(byte_pool, &_default_thread), U_SUCCESS);            // Create Default thread.
    /*
    CATCH_ERROR(_create_thread(byte_pool, &_can_thread), U_SUCCESS);                // Create CAN thread.
    CATCH_ERROR(_create_thread(byte_pool, &_faults_thread), U_SUCCESS);             // Create Faults thread.
    CATCH_ERROR(_create_thread(byte_pool, &_statemachine_thread), U_SUCCESS);       // Create State Machine thread.
    CATCH_ERROR(_create_thread(byte_pool, &_nonfunctional_data_thread), U_SUCCESS); // Create Nonfunctional Data thread.
    CATCH_ERROR(_create_thread(byte_pool, &_tsms_thread), U_SUCCESS);               // Create TSMS thread.
    CATCH_ERROR(_create_thread(byte_pool, &_control_thread), U_SUCCESS);            // Create Control thread.
    CATCH_ERROR(_create_thread(byte_pool, &_pedals_thread), U_SUCCESS);             // Create Pedals thread.
    */
    // add more threads here if need

    DEBUG_PRINTLN("Ran threads_init().");
    return U_SUCCESS;
}

