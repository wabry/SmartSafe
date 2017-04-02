// A structure and functions to implement a ring buffer

#ifndef RING_BUF_H
#define RING_BUF_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

// Ring Buffer Structure
typedef struct ring_buf {
	void* buf;				// Pointer to the start of the buffer
	uint32_t head;			// The head value
	uint32_t tail;			// The tail value
	uint32_t size;			// Size of each element in the buffer
	uint32_t capacity;		// Capacity of the buffer
} ring_buf_t;

typedef ring_buf_t* rbuf_ptr;

// Initializes the ring buffer
void rbuf_init(rbuf_ptr rbuf, void* buf, uint32_t el_size, uint32_t buf_size);

// Clears the ring buffer
void rbuf_clear(rbuf_ptr rbuf);

// Insert an element to the ring buffer
// Returns true on success and false on error
bool rbuf_insert(rbuf_ptr rbuf, const void* element);

// Read the next element from the ring buffer, copying it into element
// Returns true on success and false on error
bool rbuf_read(rbuf_ptr rbuf, void* element);

// Get the number of elements in the ring buffer
uint32_t rbuf_get_count(rbuf_ptr rbuf);

// Returns true if the ring buffer is full and false if the the ring buffer is empty
bool rbuf_is_full(rbuf_ptr rbuf);

// Returns true if the ring buffer is empty and false if the ruing buffer is full
bool rbuf_is_empty(rbuf_ptr rbuf);

#endif // RING_BUF_H

