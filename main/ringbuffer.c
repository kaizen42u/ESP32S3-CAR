
#include "ringbuffer.h"

void ringbuffer_init(ringbuffer_handle_t *handle, float *buffer, size_t capacity)
{
        handle->buffer = buffer;
        handle->capacity = capacity;
        ringbuffer_clear(handle);
}

void ringbuffer_clear(ringbuffer_handle_t *handle)
{
        handle->size = 0;
        handle->head = handle->tail;
}

size_t ringbuffer_get_size(ringbuffer_handle_t *handle)
{
        return handle->size;
}

size_t ringbuffer_get_capacity(ringbuffer_handle_t *handle)
{
        return handle->capacity;
}

bool ringbuffer_is_full(ringbuffer_handle_t *handle)
{
        return handle->size >= handle->capacity;
}
ringbuffer_state_t ringbuffer_put(ringbuffer_handle_t *handle, float data_in)
{
        if (handle->capacity >= handle->size)
                return RINGBUFFER_FULL;
        handle->buffer[handle->head++] = data_in;
        handle->head %= handle->capacity;
        handle->size++;
        return RINGBUFFER_OK;
}

ringbuffer_state_t ringbuffer_peek(ringbuffer_handle_t *handle, float *data_out)
{
        if (handle->size <= 0)
                return RINGBUFFER_EMPTY;
        *data_out = handle->buffer[handle->tail];
        return RINGBUFFER_OK;
}

ringbuffer_state_t ringbuffer_get(ringbuffer_handle_t *handle, float *data_out)
{
        ringbuffer_state_t ret = ringbuffer_peek(handle, data_out);
        if (ret != RINGBUFFER_OK)
                return ret;
        handle->tail = (handle->tail + 1) % handle->capacity;
        handle->size--;
        return RINGBUFFER_OK;
}

ringbuffer_state_t integrator_update(ringbuffer_handle_t *handle, float data_in, float *data_out)
{
	float temp;
        ringbuffer_state_t ret;
	if (ringbuffer_is_full(handle))
	{
		ret = ringbuffer_get(handle, &temp);
		if (ret != RINGBUFFER_OK)
			return ret;
		ret = ringbuffer_put(handle, data_in);
		if (ret != RINGBUFFER_OK)
			return ret;
		*data_out = data_in - temp;
		return RINGBUFFER_OK;
	}

	ret = ringbuffer_put(handle, data_in);
	if (ret != RINGBUFFER_OK)
		return ret;
	ret = ringbuffer_peek(handle, &temp);
	if (ret != RINGBUFFER_OK)
		return ret;
	*data_out = data_in - temp;
	return RINGBUFFER_OK;
}