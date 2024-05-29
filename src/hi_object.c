/**
 * @file hi_object.c
 * @author Cosmade (deepskystar@outlook.com)
 * @brief 
 * @version
 * @date 2024-05-06
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

#include "hi_sys.h"
#include "hi_object.h"
#include "hi_memory.h"

const hi_class _hi_object_class = {
    .super = NULL,
    .name = "hi_object",
    .size = sizeof(hi_object),
    .deinit_func = NULL
};

inline hi_class* hi_class_from_object(hi_object *object)
{
    return object->_meta;
}

hi_object* hi_object_from_ref(hi_object_ref_t *ref);
inline hi_object* hi_object_from_ref(hi_object_ref_t *ref) 
{
    return (void*)ref + sizeof(hi_object_ref_t);
}

hi_object_ref_t* hi_object_ref(hi_object *object);
inline hi_object_ref_t* hi_object_ref(hi_object *object) 
{
    return (void*)object - sizeof(hi_object_ref_t);
}

inline void* hi_object_new(hi_class* class_inf)
{
    hi_object_ref_t *ref = hi_malloc(class_inf->size + sizeof(hi_object_ref_t));
    HI_SETZERO(ref->state);
    ref->ref_cnt = 1;
    hi_object_from_ref(ref)->_meta = class_inf;
    return hi_object_from_ref(ref);
}

inline void* hi_object_retain(hi_object *object)
{
    ++(hi_object_ref(object)->ref_cnt);
    return object;
}

inline void* hi_object_release(hi_object *object)
{
    if (hi_object_ref(object)->ref_cnt <= 0) {
        HI_LOGE("release <%zx> error, ref_cnt is 0.\n", (hi_size_t)object);
        return NULL;
    }
    if (--(hi_object_ref(object)->ref_cnt) <= 0) {
        object->_meta->deinit_func(object);
        hi_free(hi_object_ref(object));
        return NULL;
    }
    return object;
}

inline void* hi_object_copying(hi_object *src, hi_object *dist)
{
    memcpy(dist, src, src->_meta->size);
    return dist;
}

void hi_object_mark_weak(hi_object* object)
{
    //TODO:
}

void hi_object_unmark_weak(hi_object* object)
{
    //TODO:
}

inline hi_weakptr_t hi_object_weak_ptr_new(hi_object* object)
{
    if (object == NULL) return (hi_weakptr_t)NULL;
    if (!hi_object_ref(object)->state.has_weakref) {
        hi_object_mark_weak(object);
    }
    return (hi_weakptr_t)object;
}

inline void* hi_object_weak_ptr_get(hi_weakptr_t ptr)
{
    if (ptr == 0) return NULL;
    //TODO:
    return NULL;
}

void* hi_object_delay_release(hi_object *object)
{
    //TODO:
    return NULL;
}

void hi_object_pool_drain(hi_object *object)
{
    //TODO:
}
