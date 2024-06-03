/**
 * @file hi_queue.c
 * @author Cosmade (deepskystar@outlook.com)
 * @brief 
 * @version
 * @date 2024-05-28
 * 
 * @copyright Copyright 2024 Cosmade
 * 
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include "hi_queue.h"

#define _HI_QUEUE_NODE(__queue__, __iter__) ((hi_queue_node_t *)hi_mem_block_pool_get((__queue__)->pool, __iter__))
#define _HI_QUEUE_HEAD(__queue__) _HI_QUEUE_NODE(__queue__, (__queue__)->head)
#define _HI_QUEUE_LAST(__queue__) _HI_QUEUE_NODE(__queue__, (__queue__)->last)

inline void hi_queue_init(hi_queue_t *queue, hi_mem_block_pool_t *pool)
{
    queue->pool = pool;
    queue->head = HI_ITER_NULL;
    queue->last = HI_ITER_NULL;
}

inline hi_result_t hi_queue_in(hi_queue_t *queue, hi_value_t value)
{
    if (queue == NULL) return HI_RESULT_MAKE_FAILED(HI_FAILED_REASON_NULL_PTR);
    if (queue->pool == NULL || queue->pool->config.block_size < sizeof(hi_queue_node_t))
    {
        return HI_RESULT_MAKE_FAILED(HI_FAILED_REASON_OUT_OF_MEMORY);
    }
    if (queue->last == HI_ITER_NULL)
    {
        queue->last = hi_mem_block_pool_take(queue->pool);
        queue->head = queue->last;
        if (queue->last == HI_ITER_NULL) return HI_RESULT_MAKE_FAILED(HI_FAILED_REASON_OUT_OF_MEMORY);
    }
    else
    {
        _HI_QUEUE_LAST(queue)->next = hi_mem_block_pool_take(queue->pool);
        if (_HI_QUEUE_LAST(queue)->next == HI_ITER_NULL) return HI_RESULT_MAKE_FAILED(HI_FAILED_REASON_OUT_OF_MEMORY);
        queue->last = _HI_QUEUE_LAST(queue)->next;
    }

    _HI_QUEUE_LAST(queue)->next = HI_ITER_NULL;
    _HI_QUEUE_LAST(queue)->value = value;
    return HI_RESULT_MAKE_OK;
}

inline void hi_queue_out(hi_queue_t *queue)
{
    if (queue == NULL || queue->pool == NULL || queue->head == HI_ITER_NULL) return ;

    hi_iter_t iter = queue->head;
    queue->head = _HI_QUEUE_HEAD(queue)->next;
    if (queue->head == HI_ITER_NULL) queue->last = HI_ITER_NULL;
    hi_mem_block_pool_bring(queue->pool, iter);
}

inline uint8_t hi_queue_empty(hi_queue_t *queue)
{
    if (queue == NULL || queue->last == HI_ITER_NULL || queue->head == HI_ITER_NULL) return 1;
    return 0;
}

inline hi_value_t hi_queue_head(hi_queue_t *queue)
{
    if (queue == NULL || queue->last == HI_ITER_NULL) return HI_VALUE_NULL;
    return _HI_QUEUE_HEAD(queue)->value;
}

inline hi_value_t hi_queue_last(hi_queue_t *queue)
{
    if (queue == NULL || queue->last == HI_ITER_NULL) return HI_VALUE_NULL;
    return _HI_QUEUE_LAST(queue)->value;
}

inline hi_iter_t hi_queue_begin(hi_queue_t *queue)
{
    return queue->head;
}

inline hi_queue_node_t hi_queue_next(hi_queue_t *queue, hi_iter_t iter)
{
    if (iter == HI_ITER_NULL) return HI_QUEUE_NODE_NULL;
    if (queue == NULL) return HI_QUEUE_NODE_NULL;
    return *_HI_QUEUE_NODE(queue, iter);
}

inline void hi_queue_del_next(hi_queue_t *queue, hi_iter_t iter)
{
    if (iter == HI_ITER_NULL) return;
    if (queue == NULL) return;
    hi_iter_t next = _HI_QUEUE_NODE(queue, iter)->next;
    _HI_QUEUE_NODE(queue, iter)->next = _HI_QUEUE_NODE(queue, next)->next;
    if (_HI_QUEUE_NODE(queue, iter)->next == HI_ITER_NULL) queue->last = iter;
    hi_mem_block_pool_bring(queue->pool, next);
}

inline void hi_queue_deinit(hi_queue_t *queue)
{
    if (queue == NULL) return;
    while (queue->last != HI_ITER_NULL)
    {
        hi_queue_out(queue);
    }
}

void hi_async_queue_init(hi_async_queue_t *queue, hi_mem_block_pool_t *pool)
{
    hi_mutex_init(&(queue->mutex));
    hi_mutex_lock(&(queue->mutex));
    hi_queue_init(&(queue->unsafe), pool);
    hi_mutex_unlock(&(queue->mutex));
}

hi_result_t hi_async_queue_in(hi_async_queue_t *queue, hi_value_t value)
{
    hi_result_t result;
    hi_mutex_lock(&(queue->mutex));
    result = hi_queue_in(&(queue->unsafe), value);
    hi_mutex_unlock(&(queue->mutex));
    return result;
}

void hi_async_queue_out(hi_async_queue_t *queue)
{
    hi_mutex_lock(&(queue->mutex));
    hi_queue_out(&(queue->unsafe));
    hi_mutex_unlock(&(queue->mutex));
}

uint8_t hi_async_queue_empty(hi_async_queue_t *queue)
{
    uint8_t result;
    hi_mutex_lock(&(queue->mutex));
    result = hi_queue_empty(&(queue->unsafe));
    hi_mutex_unlock(&(queue->mutex));
    return result;
}

hi_value_t hi_async_queue_head(hi_async_queue_t *queue)
{
    hi_value_t value;
    hi_mutex_lock(&(queue->mutex));
    value = hi_queue_head(&(queue->unsafe));
    hi_mutex_unlock(&(queue->mutex));
    return value;
}

hi_value_t hi_async_queue_last(hi_async_queue_t *queue)
{
    hi_value_t value;
    hi_mutex_lock(&(queue->mutex));
    value = hi_queue_last(&(queue->unsafe));
    hi_mutex_unlock(&(queue->mutex));
    return value;
}

hi_iter_t hi_async_queue_begin(hi_async_queue_t *queue)
{
    hi_iter_t iter;
    hi_mutex_lock(&(queue->mutex));
    iter = hi_queue_begin(&(queue->unsafe));
    hi_mutex_unlock(&(queue->mutex));
    return iter;
}

hi_queue_node_t hi_async_queue_next(hi_async_queue_t *queue, hi_iter_t iter)
{
    hi_queue_node_t node;
    hi_mutex_lock(&(queue->mutex));
    node = hi_queue_next(&(queue->unsafe), iter);
    hi_mutex_unlock(&(queue->mutex));
    return node;
}

void hi_async_queue_del_next(hi_async_queue_t *queue, hi_iter_t iter)
{
    hi_mutex_lock(&(queue->mutex));
    hi_queue_next(&(queue->unsafe), iter);
    hi_mutex_unlock(&(queue->mutex));
}

void hi_async_queue_deinit(hi_async_queue_t *queue)
{
    hi_mutex_lock(&(queue->mutex));
    hi_queue_deinit(&(queue->unsafe));
    hi_mutex_unlock(&(queue->mutex));
    hi_mutex_deinit(&(queue->mutex));
}