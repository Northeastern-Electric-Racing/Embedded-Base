
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

/* Checks if a mutex is owned by the current thread. */
bool mutex_isOwned(mutex_t *mutex) {
	TX_THREAD* mutex_thread; // Thread that currently owns the mutex.
	uint8_t status = tx_mutex_info_get(&mutex->_TX_MUTEX, TX_NULL, TX_NULL, &mutex_thread, TX_NULL, TX_NULL, TX_NULL);
	if(status != TX_SUCCESS) {
		PRINTLN_ERROR("Failed to call tx_mutex_info_get() (Status: %d/%s, Mutex: %s).", status, tx_status_toString(status), mutex->name);
		return false;
	}

	TX_THREAD* current_thread = tx_thread_identify(); // The current active thread (i.e., the thread that called mutex_isOwned()).
	return (mutex_thread == current_thread); // If mutex_thread is the same as current_thread, return true. If not, return false.
}

// clang-format on