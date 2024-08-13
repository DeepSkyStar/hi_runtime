#include "hi_buffer.h"
#include "hi_memory.h"

inline hi_buffer_t* hi_buffer_new(hi_size_t size)
{
    hi_buffer_t *buffer = hi_malloc(sizeof(hi_buffer_t));
    buffer->is_dynamic = 1;
    buffer->max_size = size;
    buffer->cur_size = 0;
    buffer->cur = 0;
    buffer->data = hi_malloc(size);
    return buffer;
}

inline void hi_buffer_init(hi_buffer_t *buffer)
{
    hi_mutex_init(&(buffer->mutex));
}

inline hi_size_t hi_buffer_add(hi_buffer_t *buffer, const uint8_t *data, hi_size_t size)
{
    //TODO: currently for fast test, so just add simple protect.
    if (size > buffer->max_size - buffer->cur_size) return 0;
    size = buffer->cur_size + size > buffer->max_size?buffer->max_size - buffer->cur_size:size;
    if (size > 0)
    {
        hi_memcpy(hi_buffer_last(buffer), data, size);
        buffer->cur_size = buffer->cur_size + size;
    }
    return size;
}

inline void hi_buffer_lock(hi_buffer_t *buffer)
{
    hi_mutex_lock(&(buffer->mutex));
}

inline void hi_buffer_unlock(hi_buffer_t *buffer)
{
    hi_mutex_unlock(&(buffer->mutex));
}

inline void hi_buffer_set_data(hi_buffer_t *buffer, const uint8_t *data, hi_size_t size)
{
    //TODO: should be optimize.
    size = size < buffer->max_size ? size : buffer->max_size;
    hi_memmove(buffer->data, data, size);
    buffer->cur = 0;
    buffer->cur_size = size;
}

inline void hi_buffer_clear(hi_buffer_t *buffer)
{
    buffer->cur = 0;
    buffer->cur_size = 0;
}

inline uint8_t* hi_buffer_begin(hi_buffer_t *buffer)
{
    return buffer->data + buffer->cur;
}

inline uint8_t* hi_buffer_last(hi_buffer_t *buffer)
{
    return buffer->data + buffer->cur_size;
}

inline hi_size_t hi_buffer_length(hi_buffer_t *buffer)
{
    if (buffer->cur_size <= buffer->cur) return 0;
    return buffer->cur_size - buffer->cur;
}

inline hi_size_t hi_buffer_remain(hi_buffer_t *buffer)
{
    return buffer->max_size - buffer->cur_size;
}

inline void hi_buffer_clear_begin(hi_buffer_t *buffer)
{
    //TODO: should be optimize.
    if (hi_buffer_length(buffer) > 0)
    {
        hi_memmove(buffer->data, hi_buffer_begin(buffer), hi_buffer_length(buffer));
    }
    buffer->cur_size = hi_buffer_length(buffer);
    buffer->cur = 0;
}

inline void hi_buffer_deinit(hi_buffer_t *buffer)
{
    hi_mutex_deinit(&(buffer->mutex));
}

inline void hi_buffer_free(hi_buffer_t *buffer)
{
    hi_buffer_deinit(buffer);
    if (buffer->is_dynamic)
    {
        hi_free(buffer->data);
        buffer->data = NULL;
    }
    hi_free(buffer);
}
