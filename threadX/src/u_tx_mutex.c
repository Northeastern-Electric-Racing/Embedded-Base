
#include "u_tx_mutex.h"

// clang-format off

/* Helper function. Creates a ThreadX mutex. */
uint8_t create_mutex(mutex_t *mutex)
{
	uint8_t status = tx_mutex_create(&mutex->_TX_MUTEX, (CHAR *)mutex->name, mutex->priority_inherit);
	if (status != TX_SUCCESS) {
		PRINTLN_ERROR("Failed to create mutex (Status: %d/%s, Name: %s).",status, tx_status_toString(status), mutex->name);
		return status;
	}

	return U_SUCCESS;
}

/* Get a mutex. */
uint8_t mutex_get(mutex_t *mutex)
{
	uint8_t status = tx_mutex_get(&mutex->_TX_MUTEX, MUTEX_WAIT_TIME);
	if (status != TX_SUCCESS) {
		PRINTLN_ERROR("Failed to get mutex (Status: %d/%s, Mutex: %s).", status, tx_status_toString(status), mutex->name);
		return status;
	}

	return U_SUCCESS;
}

/* Put a mutex. */
uint8_t mutex_put(mutex_t *mutex)
{
	uint8_t status = tx_mutex_put(&mutex->_TX_MUTEX);
	if (status != TX_SUCCESS) {
		PRINTLN_ERROR("Failed to put mutex (Status: %d/%s, Mutex: %s).", status, tx_status_toString(status), mutex->name);
		return status;
	}

	return U_SUCCESS;
}

// clang-format on