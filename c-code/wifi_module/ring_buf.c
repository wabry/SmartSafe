#include "ring_buf.h"

// Initializes the ring buffer
void rbuf_init(rbuf_ptr rbuf, void* buf, uint32_t element_size, uint32_t buf_length)
{
	rbuf->buf = buf;
	rbuf->size = element_size;
	rbuf->capacity = buf_length;
	rbuf->head = 0;
	rbuf->tail = 0;
}

// Clears the ring buffer
void rbuf_clear(rbuf_ptr rbuf)
{
	rbuf->head = 0;
	rbuf->tail = 0;
}

// Insert an element to the ring buffer
// Returns true on success or false on error
bool rbuf_insert(rbuf_ptr rbuf, const void* element)
{
	uint32_t head_index = (rbuf->head) % (rbuf->capacity);
	memcpy((void*)((uint32_t)(rbuf->buf) + head_index), element, rbuf->size);
	rbuf->head++;
	return true;
}

// Read the next element from the ring buffer, copying it into element
// Returns true on success or false on error
bool rbuf_read(rbuf_ptr rbuf, void* element)
{
	if (rbuf_is_empty(rbuf))
	{
		return false;
	}
	uint32_t tail_index = (rbuf->tail) % (rbuf->capacity);
	memcpy(element, (void*)((uint32_t)(rbuf->buf) + tail_index), rbuf->size);
	rbuf->tail++;
	return true;
}

// Get the number of elements in the ring buffer
uint32_t rbuf_get_count(rbuf_ptr rbuf)
{
	return (rbuf->head - rbuf->tail);
}

// Returns true if the ring buffer is full and false if the the ring buffer is empty
bool rbuf_is_full(rbuf_ptr rbuf)
{
	if ((rbuf_get_count(rbuf)) == rbuf->capacity)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Returns true if the ring buffer is empty and false if the ruing buffer is full
bool rbuf_is_empty(rbuf_ptr rbuf)
{
	if ((rbuf_get_count(rbuf)) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
