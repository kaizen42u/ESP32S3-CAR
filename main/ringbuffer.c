
#include "ringbuffer.h"

static const char *TAG = "ringbuffer";

void ringbuffer_init(ringbuffer_handle_t *handle, float *buffer, size_t capacity)
{
        if ((handle == NULL) || (buffer == NULL) || (capacity == 0))
        {
                LOG_ERROR("NULL pointer, handle=0x%X, buffer=0x%X, capacity=%d", (uintptr_t)handle, (uintptr_t)buffer, capacity);
                return;
        }

        handle->buffer = buffer;
        handle->capacity = capacity;
        ringbuffer_clear(handle);
}

void ringbuffer_clear(ringbuffer_handle_t *handle)
{
        if (handle == NULL)
        {
                LOG_ERROR("NULL pointer, handle=0x%X", (uintptr_t)handle);
                return;
        }

        handle->size = 0;
        handle->head = handle->tail;
}

size_t ringbuffer_get_size(ringbuffer_handle_t *handle)
{
        if (handle == NULL)
        {
                LOG_ERROR("NULL pointer, handle=0x%X", (uintptr_t)handle);
                return 0;
        }

        return handle->size;
}

size_t ringbuffer_get_capacity(ringbuffer_handle_t *handle)
{
        if (handle == NULL)
        {
                LOG_ERROR("NULL pointer, handle=0x%X", (uintptr_t)handle);
                return 0;
        }

        return handle->capacity;
}

bool ringbuffer_is_full(ringbuffer_handle_t *handle)
{
        if (handle == NULL)
        {
                LOG_ERROR("NULL pointer, handle=0x%X", (uintptr_t)handle);
                return true;
        }

        return handle->size >= handle->capacity;
}
ringbuffer_state_t ringbuffer_put(ringbuffer_handle_t *handle, float data_in)
{
        if ((handle == NULL) || (handle->buffer == NULL))
        {
                LOG_ERROR("NULL pointer, handle=0x%X, handle->buffer=0x%X", (uintptr_t)handle, (uintptr_t)handle->buffer);
                return RINGBUFFER_ARGUMENT_ERROR;
        }

        if (handle->size >= handle->capacity)
                return RINGBUFFER_FULL;
        handle->buffer[handle->head++] = data_in;
        handle->head %= handle->capacity;
        handle->size++;
        return RINGBUFFER_OK;
}

ringbuffer_state_t ringbuffer_fill(ringbuffer_handle_t *handle, float data_in)
{
        if ((handle == NULL) || (handle->buffer == NULL))
        {
                LOG_ERROR("NULL pointer, handle=0x%X, handle->buffer=0x%X", (uintptr_t)handle, (uintptr_t)handle->buffer);
                return RINGBUFFER_ARGUMENT_ERROR;
        }

        ringbuffer_state_t ret;
        while (handle->size < handle->capacity)
        {
                ret = ringbuffer_put(handle, data_in);
                if (ret != RINGBUFFER_OK)
                        return ret;
        }

        return RINGBUFFER_OK;
}

ringbuffer_state_t ringbuffer_peek(ringbuffer_handle_t *handle, float *data_out)
{
        if ((handle == NULL) || (handle->buffer == NULL) || (data_out == NULL))
        {
                LOG_ERROR("NULL pointer, handle=0x%X, handle->buffer=0x%X, data_out=0x%X", (uintptr_t)handle, (uintptr_t)handle->buffer, (uintptr_t)data_out);
                return RINGBUFFER_ARGUMENT_ERROR;
        }

        if (handle->size <= 0)
                return RINGBUFFER_EMPTY;
        *data_out = handle->buffer[handle->tail];
        return RINGBUFFER_OK;
}

ringbuffer_state_t ringbuffer_get(ringbuffer_handle_t *handle, float *data_out)
{
        if ((handle == NULL) || (handle->buffer == NULL) || (data_out == NULL))
        {
                LOG_ERROR("NULL pointer, handle=0x%X, handle->buffer=0x%X, data_out=0x%X", (uintptr_t)handle, (uintptr_t)handle->buffer, (uintptr_t)data_out);
                return RINGBUFFER_ARGUMENT_ERROR;
        }

        ringbuffer_state_t ret = ringbuffer_peek(handle, data_out);
        if (ret != RINGBUFFER_OK)
        {
                LOG_WARNING("return state: %s", RINGBUFFER_STATE_STRING[ret]);
                return ret;
        }
        handle->tail++;
        handle->tail %= handle->capacity;
        handle->size--;
        return RINGBUFFER_OK;
}

ringbuffer_state_t differentiator_update(ringbuffer_handle_t *handle, float data_in, float *data_out)
{
        if ((handle == NULL) || (handle->buffer == NULL) || (data_out == NULL))
        {
                LOG_ERROR("NULL pointer, handle=0x%X, handle->buffer=0x%X, data_out=0x%X", (uintptr_t)handle, (uintptr_t)handle->buffer, (uintptr_t)data_out);
                return RINGBUFFER_ARGUMENT_ERROR;
        }

        float temp;
        ringbuffer_state_t ret;
        if (ringbuffer_is_full(handle))
        {
                ret = ringbuffer_get(handle, &temp);
                if (ret != RINGBUFFER_OK)
                {
                        LOG_WARNING("return state: %s", RINGBUFFER_STATE_STRING[ret]);
                        return ret;
                }
                ret = ringbuffer_put(handle, data_in);
                if (ret != RINGBUFFER_OK)
                {
                        LOG_WARNING("return state: %s", RINGBUFFER_STATE_STRING[ret]);
                        return ret;
                }
                *data_out = data_in - temp;
                return RINGBUFFER_OK;
        }

        ret = ringbuffer_put(handle, data_in);
        if (ret != RINGBUFFER_OK)
        {
                LOG_WARNING("return state: %s", RINGBUFFER_STATE_STRING[ret]);
                return ret;
        }
        ret = ringbuffer_peek(handle, &temp);
        if (ret != RINGBUFFER_OK)
        {
                LOG_WARNING("return state: %s", RINGBUFFER_STATE_STRING[ret]);
                return ret;
        }
        *data_out = data_in - temp;
        return RINGBUFFER_OK;
}