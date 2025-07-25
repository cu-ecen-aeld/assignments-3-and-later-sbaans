/**
 * @file aesd-circular-buffer.c
 * @brief Functions and data related to a circular buffer imlementation
 *
 * @author Dan Walkes
 * @date 2020-03-01
 * @copyright Copyright (c) 2020
 *
 */
#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#endif

#include "aesd-circular-buffer.h"

/**
 * @param buffer the buffer to search for corresponding offset.  Any necessary locking must be performed by caller.
 * @param char_offset the position to search for in the buffer list, describing the zero referenced
 *      character index if all buffer strings were concatenated end to end
 * @param entry_offset_byte_rtn is a pointer specifying a location to store the byte of the returned aesd_buffer_entry
 *      buffptr member corresponding to char_offset.  This value is only set when a matching char_offset is found
 *      in aesd_buffer.
 * @return the struct aesd_buffer_entry structure representing the position described by char_offset, or
 * NULL if this position is not available in the buffer (not enough data is written).
 */
struct aesd_buffer_entry *aesd_circular_buffer_find_entry_offset_for_fpos(struct aesd_circular_buffer *buffer,
            size_t char_offset, size_t *entry_offset_byte_rtn )
{
    /**
    * TODO: implement per description
    */
	struct aesd_buffer_entry *current_entry = NULL;
	size_t current_position = 0;
	uint8_t write_ptr = 0;
	uint8_t read_ptr = 0;;



	uint8_t index;

	/* Get the current read and write pointers */
	write_ptr = buffer->in_offs;
	read_ptr = buffer->out_offs;

	/* Verify if it is not empty */
	if ( (write_ptr == read_ptr) && (buffer->full == false)) {
		return NULL;
	}

	/* Loop to find the right position */
	index = read_ptr;

	do {
		current_entry = &(buffer->entry[index]);

		/* Test if it is the good entry */
		if ( (current_position + current_entry->size) > char_offset) {
			*entry_offset_byte_rtn = ( char_offset - current_position );
			return current_entry;
		} else {
			current_position += current_entry->size;
			index = (index + 1)%AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED;
		}

	} while (index != write_ptr);

    return NULL;
}

/**
* Adds entry @param add_entry to @param buffer in the location specified in buffer->in_offs.
* If the buffer was already full, overwrites the oldest entry and advances buffer->out_offs to the
* new start location.
* Any necessary locking must be handled by the caller
* Any memory referenced in @param add_entry must be allocated by and/or must have a lifetime managed by the caller.
*/
void aesd_circular_buffer_add_entry(struct aesd_circular_buffer *buffer, const struct aesd_buffer_entry *add_entry)
{
    /**
    * TODO: implement per description
    */

	/* Writing at in pointer */
	(buffer->entry[buffer->in_offs]).size = add_entry->size;
	(buffer->entry[buffer->in_offs]).buffptr = add_entry->buffptr;
	(buffer->entry[buffer->in_offs]).entry_index = buffer->in_offs;

	/* Increment in pointer */

	(buffer->in_offs) = ((buffer->in_offs) + 1)%AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED;

	/* If buffer was full, out pointer should be set equal to in pointer */
	if (buffer->full == true) {
		(buffer->out_offs) = (buffer->in_offs);
	}


	/* If in pointer is equal to out pointer and buffer was not full then it is full now */
	if ( ( (buffer->out_offs) == (buffer->in_offs) )&& (buffer->full == false)) {
		buffer->full = true;
	}
}

/**
* Initializes the circular buffer described by @param buffer to an empty struct
*/
void aesd_circular_buffer_init(struct aesd_circular_buffer *buffer)
{
    memset(buffer,0,sizeof(struct aesd_circular_buffer));
}
