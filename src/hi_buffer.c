#include "hi_buffer.h"
#include "hi_memory.h"

void hi_buffer_init(hi_buffer_t *buffer)
{
    hi_mutex_init(&(buffer->mutex));
}

hi_size_t hi_buffer_add(hi_buffer_t *buffer, const uint8_t *data, hi_size_t size)
{
    size = buffer->cur_size + size > buffer->max_size?buffer->max_size - buffer->cur_size:size;
    if (size > 0)   
    {
        hi_memcpy(buffer->data + buffer->cur, data, size);
        buffer->cur = buffer->cur + size;
        buffer->cur_size = buffer->cur_size + size;
    }
    return size;
}

void hi_buffer_lock(hi_buffer_t *buffer)
{
    hi_mutex_lock(&(buffer->mutex));
}

void hi_buffer_unlock(hi_buffer_t *buffer)
{
    hi_mutex_unlock(&(buffer->mutex));
}

void hi_buffer_set_data(hi_buffer_t *buffer, const uint8_t *data, hi_size_t size)
{
    //TODO: should be optimize.
    size = size < buffer->max_size ? size : buffer->max_size;
    hi_memmove(buffer->data, data, size);
    buffer->cur = 0;
    buffer->cur_size = size;
}

void hi_buffer_clear(hi_buffer_t *buffer)
{
    buffer->cur = 0;
    buffer->cur_size = 0;
}

void hi_buffer_deinit(hi_buffer_t *buffer)
{
    hi_mutex_deinit(&(buffer->mutex));
}