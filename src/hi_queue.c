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

inline hi_queue_t* hi_queue_new(hi_size_t data_size, hi_size_t max_size)
{
    hi_queue_t *queue = hi_malloc(sizeof(hi_queue_t));
    queue->pool = hi_mem_pool_new((hi_mem_pool_config_t){
        .use_check = 1,
        .is_dynamic = 1,
        .block_size = sizeof(hi_queue_node_t) + data_size,
        .block_count = max_size
    });
    return queue;
}

inline void hi_queue_init(hi_queue_t *queue)
{
    queue->first = HI_ITER_NULL;
    queue->last = HI_ITER_NULL;
    queue->usage = 0;
}

inline void hi_queue_deinit(hi_queue_t *queue)
{
    hi_queue_del_all(queue);
    hi_mem_pool_free(queue->pool);
    queue->pool = NULL;
}

inline void hi_queue_free(hi_queue_t *queue)
{
    hi_queue_deinit(queue);
    hi_free(queue);
}

inline hi_iter_t hi_queue_try_add(hi_queue_t *queue)
{
    if (queue == NULL || queue->pool == NULL) return HI_ITER_NULL;
    return hi_mem_pool_try(queue->pool);
}

inline hi_iter_t hi_queue_add_first(hi_queue_t *queue, const void* data, hi_size_t size)
{
    if (queue == NULL || queue->pool == NULL) return HI_ITER_NULL;
    if (queue->pool->config.block_size < size){
        HI_LOGE("the size is bigger then queue block size!");
        return HI_ITER_NULL;
    }

    hi_iter_t iter = hi_mem_pool_take(queue->pool);
    if (iter == HI_ITER_NULL) {
        // HI_LOGE("queue mem pool is full!");
        return HI_ITER_NULL;
    }
    queue->usage++;
    hi_queue_get_node(queue, iter)->prev = HI_ITER_NULL;
    hi_queue_get_node(queue, iter)->next = queue->first;
    hi_memcpy(hi_queue_get_node(queue, iter)->data, data, size);

    if (queue->first == HI_ITER_NULL)
    {
        queue->first = iter;
        queue->last = iter;
        return iter;
    }

    hi_queue_get_node(queue, queue->first)->prev = iter;
    queue->first = iter;
    return iter;
}

inline hi_iter_t hi_queue_add_first_value(hi_queue_t *queue, hi_value_t value)
{
    return hi_queue_add_first(queue, &value, sizeof(value));
}

inline hi_iter_t hi_queue_add_last(hi_queue_t *queue, const void* data, hi_size_t size)
{
    if (queue == NULL || queue->pool == NULL) return HI_ITER_NULL;
    if (queue->pool->config.block_size < size){
        HI_LOGE("the size is bigger then queue block size!");
        return HI_ITER_NULL;
    }

    hi_iter_t iter = hi_mem_pool_take(queue->pool);
    if (iter == HI_ITER_NULL) {
        // HI_LOGE("queue mem pool is full!");
        return HI_ITER_NULL;
    }
    queue->usage++;
    hi_queue_get_node(queue, iter)->prev = queue->last;
    hi_queue_get_node(queue, iter)->next = HI_ITER_NULL;
    hi_memcpy(hi_queue_get_node(queue, iter)->data, data, size);

    if (queue->last == HI_ITER_NULL)
    {
        queue->first = iter;
        queue->last = iter;
        return iter;
    }

    hi_queue_get_node(queue, queue->last)->next = iter;
    queue->last = iter;
    return iter;
}

inline hi_iter_t hi_queue_add_last_value(hi_queue_t *queue, hi_value_t value)
{
    return hi_queue_add_last(queue, &value, sizeof(value));
}

inline hi_iter_t hi_queue_insert_next(hi_queue_t *queue, hi_iter_t iter, const void *data, hi_size_t size)
{
    if (queue == NULL || queue->pool == NULL) return HI_ITER_NULL;
    if (queue->pool->config.block_size < size){
        HI_LOGE("the size is bigger then queue block size!");
        return HI_ITER_NULL;
    }

    if (hi_queue_get_node(queue, iter) == NULL)
    {
        HI_LOGE("the queue iter not exist!");
        return HI_ITER_NULL;
    }

    hi_iter_t new_iter = hi_mem_pool_take(queue->pool);
    if (new_iter == HI_ITER_NULL) {
        // HI_LOGE("queue mem pool is full!");
        return HI_ITER_NULL;
    }
    queue->usage++;

    hi_queue_get_node(queue, new_iter)->prev = iter;
    hi_queue_get_node(queue, new_iter)->next = hi_queue_get_node(queue, iter)->next;
    hi_memcpy(hi_queue_get_node(queue, new_iter)->data, data, size);

    hi_queue_get_node(queue, iter)->next = new_iter;
    if (iter == queue->last) queue->last = new_iter;
    return new_iter;
}

inline hi_iter_t hi_queue_insert_next_value(hi_queue_t *queue, hi_iter_t iter, hi_value_t value)
{
    return hi_queue_insert_next(queue, iter, &value, sizeof(value));
}

inline hi_iter_t hi_queue_insert_prev(hi_queue_t *queue, hi_iter_t iter, const void *data, hi_size_t size)
{
    if (queue == NULL || queue->pool == NULL) return HI_ITER_NULL;
    if (queue->pool->config.block_size < size){
        HI_LOGE("the size is bigger then queue block size!");
        return HI_ITER_NULL;
    }

    if (hi_queue_get_node(queue, iter) == NULL)
    {
        HI_LOGE("the queue iter not exist!");
        return HI_ITER_NULL;
    }

    hi_iter_t new_iter = hi_mem_pool_take(queue->pool);
    if (new_iter == HI_ITER_NULL) {
        // HI_LOGE("queue mem pool is full!");
        return HI_ITER_NULL;
    }
    queue->usage++;

    hi_queue_get_node(queue, new_iter)->prev = hi_queue_get_node(queue, iter)->prev;
    hi_queue_get_node(queue, new_iter)->next = iter;
    hi_memcpy(hi_queue_get_node(queue, new_iter)->data, data, size);

    hi_queue_get_node(queue, iter)->prev = new_iter;
    if (iter == queue->first) queue->first = new_iter;
    return new_iter;
}

inline hi_iter_t hi_queue_insert_prev_value(hi_queue_t *queue, hi_iter_t iter, hi_value_t value)
{
    return hi_queue_insert_prev(queue, iter, &value, sizeof(value));
}

inline void* hi_queue_get(hi_queue_t *queue, hi_iter_t iter)
{
    if (hi_queue_get_node(queue, iter) == NULL) return NULL;
    return hi_queue_get_node(queue, iter)->data;
}

inline void* hi_queue_get_first(hi_queue_t *queue)
{
    if (queue == NULL) return NULL;
    return hi_queue_get(queue, queue->first);
}

inline void* hi_queue_get_last(hi_queue_t *queue)
{
    if (queue == NULL) return NULL;
    return hi_queue_get(queue, queue->last);
}

inline hi_queue_node_t* hi_queue_get_node(hi_queue_t *queue, hi_iter_t iter)
{
    if (queue == NULL) return NULL;
    return (hi_queue_node_t*)hi_mem_pool_get(queue->pool, iter);
}

inline hi_size_t hi_queue_get_usage(hi_queue_t *queue)
{
    if (queue != NULL) return queue->usage;
    return 0;
}

inline hi_size_t hi_queue_get_data_size(hi_queue_t *queue)
{
    if (queue != NULL && queue->pool != NULL) return queue->pool->config.block_size;
    return 0;
}

inline hi_size_t hi_queue_get_max_size(hi_queue_t *queue)
{
    if (queue != NULL && queue->pool != NULL) return queue->pool->config.block_count;
    return 0;
}

inline void hi_queue_del_first(hi_queue_t *queue)
{
    if (queue == NULL) return;
    hi_queue_del(queue, queue->first);
}

inline void hi_queue_del_last(hi_queue_t *queue)
{
    if (queue == NULL) return;
    hi_queue_del(queue, queue->last);
}

inline void hi_queue_del(hi_queue_t *queue, hi_iter_t iter)
{
    hi_queue_node_t *node = hi_queue_get_node(queue, iter);
    if (node == NULL && queue->usage > 0) return;
    queue->usage--;
    hi_queue_node_t *prev = hi_queue_get_node(queue, node->prev);
    hi_queue_node_t *next = hi_queue_get_node(queue, node->next);
    if (prev != NULL) prev->next = node->next;
    if (next != NULL) next->prev = node->prev;
    if (queue->first == iter) queue->first = node->next;
    if (queue->last == iter) queue->last = node->prev;

    hi_mem_pool_bring(queue->pool, iter);
}

inline void hi_queue_del_all(hi_queue_t *queue)
{
    if (queue == NULL) return ;
    queue->first = HI_ITER_NULL;
    queue->last = HI_ITER_NULL;
    queue->usage = 0;
    hi_mem_pool_bring_all(queue->pool);
}

inline hi_iter_t hi_queue_begin(hi_queue_t *queue)
{
    return queue->first;
}

inline hi_iter_t hi_queue_end(hi_queue_t *queue)
{
    return queue->last;
}

inline hi_iter_t hi_queue_next(hi_queue_t *queue, hi_iter_t iter)
{
    if (hi_queue_get_node(queue, iter) == NULL) return HI_ITER_NULL;
    return hi_queue_get_node(queue, iter)->next;
}

inline hi_iter_t hi_queue_prev(hi_queue_t *queue, hi_iter_t iter)
{
    if (hi_queue_get_node(queue, iter) == NULL) return HI_ITER_NULL;
    return hi_queue_get_node(queue, iter)->prev;
}

/************************************************* sync queue ***************************************************/

hi_sync_queue_t* hi_sync_queue_new(hi_size_t data_size, hi_size_t max_size)
{
    hi_sync_queue_t *queue = hi_malloc(sizeof(hi_sync_queue_t));
    queue->unsafe.pool = hi_mem_pool_new((hi_mem_pool_config_t){
        .use_check = 1,
        .is_dynamic = 1,
        .block_size = sizeof(hi_queue_node_t) + data_size,
        .block_count = max_size
    });
    return queue;
}

inline void hi_sync_queue_init(hi_sync_queue_t *queue)
{
    hi_mutex_init(&(queue->mutex));
    hi_mutex_lock(&(queue->mutex));
    hi_queue_init(&(queue->unsafe));
    hi_mutex_unlock(&(queue->mutex));
}

inline void hi_sync_queue_deinit(hi_sync_queue_t *queue)
{
    hi_mutex_lock(&(queue->mutex));
    hi_queue_deinit(&(queue->unsafe));
    hi_mutex_unlock(&(queue->mutex));
    hi_mutex_deinit(&(queue->mutex));
}

inline void hi_sync_queue_free(hi_sync_queue_t *queue)
{
    hi_sync_queue_deinit(queue);
    hi_free(queue);
}

inline hi_iter_t hi_sync_queue_try_add(hi_sync_queue_t *queue)
{
    hi_iter_t iter;
    hi_mutex_lock(&(queue->mutex));
    iter = hi_queue_try_add(&(queue->unsafe));
    hi_mutex_unlock(&(queue->mutex));
    return iter;
}

inline hi_iter_t hi_sync_queue_add_first(hi_sync_queue_t *queue, const void* data, hi_size_t size)
{
    hi_mutex_lock(&(queue->mutex));
    size = hi_queue_add_first(&(queue->unsafe), data, size);
    hi_mutex_unlock(&(queue->mutex));
    return size;
}

inline hi_iter_t hi_sync_queue_add_first_value(hi_sync_queue_t *queue, hi_value_t value)
{
    hi_mutex_lock(&(queue->mutex));
    value.iter = hi_queue_add_first_value(&(queue->unsafe), value);
    hi_mutex_unlock(&(queue->mutex));
    return value.iter;
}

inline hi_iter_t hi_sync_queue_add_last(hi_sync_queue_t *queue, const void* data, hi_size_t size)
{
    hi_mutex_lock(&(queue->mutex));
    size = hi_queue_add_last(&(queue->unsafe), data, size);
    hi_mutex_unlock(&(queue->mutex));
    return size;
}

inline hi_iter_t hi_sync_queue_add_last_value(hi_sync_queue_t *queue, hi_value_t value)
{
    hi_mutex_lock(&(queue->mutex));
    value.iter = hi_queue_add_last_value(&(queue->unsafe), value);
    hi_mutex_unlock(&(queue->mutex));
    return value.iter;
}

inline hi_iter_t hi_sync_queue_insert_next(hi_sync_queue_t *queue, hi_iter_t iter, const void *data, hi_size_t size)
{
    hi_mutex_lock(&(queue->mutex));
    iter = hi_queue_insert_next(&(queue->unsafe), iter, data, size);
    hi_mutex_unlock(&(queue->mutex));
    return iter;
}

inline hi_iter_t hi_sync_queue_insert_next_value(hi_sync_queue_t *queue, hi_iter_t iter, hi_value_t value)
{
    hi_mutex_lock(&(queue->mutex));
    iter = hi_queue_insert_next_value(&(queue->unsafe), iter, value);
    hi_mutex_unlock(&(queue->mutex));
    return iter;
}

inline hi_iter_t hi_sync_queue_insert_prev(hi_sync_queue_t *queue, hi_iter_t iter, const void *data, hi_size_t size)
{
    hi_mutex_lock(&(queue->mutex));
    iter = hi_queue_insert_prev(&(queue->unsafe), iter, data, size);
    hi_mutex_unlock(&(queue->mutex));
    return iter;
}

inline hi_iter_t hi_sync_queue_insert_prev_value(hi_sync_queue_t *queue, hi_iter_t iter, hi_value_t value)
{
    hi_mutex_lock(&(queue->mutex));
    iter = hi_queue_insert_prev_value(&(queue->unsafe), iter, value);
    hi_mutex_unlock(&(queue->mutex));
    return iter;
}

inline void* hi_sync_queue_get(hi_sync_queue_t *queue, hi_iter_t iter)
{
    void *data;
    hi_mutex_lock(&(queue->mutex));
    data = hi_queue_get(&(queue->unsafe), iter);
    hi_mutex_unlock(&(queue->mutex));
    return data;
}

inline void* hi_sync_queue_get_first(hi_sync_queue_t *queue)
{
    //TODO: check the thread safe.
    return hi_sync_queue_get(queue, queue->unsafe.first);
}

inline void* hi_sync_queue_get_last(hi_sync_queue_t *queue)
{
    //TODO: check the thread safe.
    return hi_sync_queue_get(queue, queue->unsafe.last);
}

inline hi_queue_node_t* hi_sync_queue_get_node(hi_sync_queue_t *queue, hi_iter_t iter)
{
    hi_queue_node_t* node;
    hi_mutex_lock(&(queue->mutex));
    node = hi_queue_get_node(&(queue->unsafe), iter);
    hi_mutex_unlock(&(queue->mutex));
    return node;
}

inline hi_size_t hi_sync_queue_get_usage(hi_sync_queue_t *queue)
{
    hi_size_t size;
    hi_mutex_lock(&(queue->mutex));
    size = hi_queue_get_usage(&(queue->unsafe));
    hi_mutex_unlock(&(queue->mutex));
    return size;
}

inline hi_size_t hi_sync_queue_get_data_size(hi_sync_queue_t *queue)
{
    hi_size_t size;
    hi_mutex_lock(&(queue->mutex));
    size = hi_queue_get_data_size(&(queue->unsafe));
    hi_mutex_unlock(&(queue->mutex));
    return size;
}

inline hi_size_t hi_sync_queue_get_max_size(hi_sync_queue_t *queue)
{
    hi_size_t size;
    hi_mutex_lock(&(queue->mutex));
    size = hi_queue_get_max_size(&(queue->unsafe));
    hi_mutex_unlock(&(queue->mutex));
    return size;
}

inline void hi_sync_queue_del_first(hi_sync_queue_t *queue)
{
    hi_mutex_lock(&(queue->mutex));
    hi_queue_del_first(&(queue->unsafe));
    hi_mutex_unlock(&(queue->mutex));
}

inline void hi_sync_queue_del_last(hi_sync_queue_t *queue)
{
    hi_mutex_lock(&(queue->mutex));
    hi_queue_del_last(&(queue->unsafe));
    hi_mutex_unlock(&(queue->mutex));
}

inline void hi_sync_queue_del(hi_sync_queue_t *queue, hi_iter_t iter)
{
    hi_mutex_lock(&(queue->mutex));
    hi_queue_del(&(queue->unsafe), iter);
    hi_mutex_unlock(&(queue->mutex));
}

inline void hi_sync_queue_del_all(hi_sync_queue_t *queue)
{
    hi_mutex_lock(&(queue->mutex));
    hi_queue_del_all(&(queue->unsafe));
    hi_mutex_unlock(&(queue->mutex));
}

inline hi_iter_t hi_sync_queue_begin(hi_sync_queue_t *queue)
{
    return queue->unsafe.first;
}

inline hi_iter_t hi_sync_queue_end(hi_sync_queue_t *queue)
{
    return queue->unsafe.last;
}

inline hi_iter_t hi_sync_queue_next(hi_sync_queue_t *queue, hi_iter_t iter)
{
    hi_mutex_lock(&(queue->mutex));
    iter = hi_queue_next(&(queue->unsafe), iter);
    hi_mutex_unlock(&(queue->mutex));
    return iter;
}

inline hi_iter_t hi_sync_queue_prev(hi_sync_queue_t *queue, hi_iter_t iter)
{
    hi_mutex_lock(&(queue->mutex));
    iter = hi_queue_prev(&(queue->unsafe), iter);
    hi_mutex_unlock(&(queue->mutex));
    return iter;
}
