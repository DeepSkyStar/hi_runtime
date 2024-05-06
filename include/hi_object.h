/**
 * @file hi_object.h
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

#ifndef HI_OBJECT_H_
#define HI_OBJECT_H_

#include "hi_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HI_CLASS_DEF

//These for define a class.
#define HI_CLASS_DEF(__class__, __super__) typedef struct _##__class__ __class__; \
extern __class__* __class__##_init(__class__ *object); \
extern void __class__##_deinit(__class__ *object); \
extern const hi_class _##__class__##_class; \
struct _##__class__{ \
    __super__ super;

//After define class.
#define HI_CLASS_END };

#define HI_CLASS_IMP(__class__, __super__) const hi_class _##__class__##_class = { \
    .super = (hi_class *)&_##__super__##_class,   \
    .name = #__class__, \
    .size = sizeof(__class__), \
    .deinit_func = (hi_deinit_func)__class__##_deinit,  \
};

#define HI_CLASS_INIT_IMP(__class__) inline __class__* __class__##_init(__class__* object)
#define HI_CLASS_INIT_IMP_EMPTY(__class__) inline __class__* __class__##_init(__class__* object) { return object; }

#define HI_CLASS_DEINIT_IMP(__class__) inline void __class__##_deinit(__class__ *object)
#define HI_CLASS_DEINIT_IMP_EMPTY(__class__) inline void __class__##_deinit(__class__ *object) {}

#define HI_CLASS_COPYING_DEF(__class__) extern __class__* _##__class__##_copying(__class__ *src, __class__* dst);
#define HI_CLASS_COPYING_IMP(__class__) inline __class__* __class__##_copying(__class__ *src, __class__* dst)

//If no pointer, can just copy all memory.
#define HI_CLASS_COPYING_IMP_NO_PTR(__class__) inline __class__* __class__##_copying(__class__ *src, __class__* dist) { \
return (__class__*)hi_object_copying((hi_object*)src, (hi_object*)dist); \
}

//These for object create and assignment.
#define HI_OBJECT_CLASS(__object__) hi_class_from_object((hi_object *)__object__)

//For create a weak ptr.
#define HI_WEAKREF(__object__) hi_object_weak_ptr((hi_object *)__object__)
//For use a weak ptr.
#define HI_WEAKPTR(__ref__, __class__) hi_object_weak_ptr((hi_object *)__object__)

#define HI_NEW(__class__) __class__##_init( (__class__ *)hi_object_new((hi_class*)&_##__class__##_class) )
#define HI_COPY(__class__, __object__) __class__##_copying(__object__, HI_NEW(__class__))

#define HI_RETAIN(__object__) hi_object_retain((hi_object*)__object__)
#define HI_RELEASE(__object__) hi_object_release((hi_object*)__object__)

#define HI_DELAY_RELEASE(__object__) hi_object_delay_release((hi_object *)__object__)

#endif /* HI_CLASS_DEF */

typedef hi_size_t hi_weakptr_t;
typedef struct _hi_class hi_class;
typedef struct _hi_object hi_object;
typedef void (*hi_deinit_func)(void *object);

struct _hi_class
{   
    hi_class *super;
    hi_str_t name;
    hi_size_t size;
    hi_deinit_func deinit_func;
};

typedef struct
{
    uint8_t has_weakref:1;
    uint8_t has_extra_obj:1;
    uint8_t is_delay_release:1;
    uint8_t reserved:5;
}hi_object_state;

typedef struct hi_object_ref_s
{
    hi_object_state state;    //the type of object.
    int32_t ref_cnt;  //when ref_cnt down to 0, and delay_release_cnt is 0, the object will be release.
}hi_object_ref_t;

struct _hi_object
{
    hi_class* _meta; //the class info for the object.
};

extern const hi_class _hi_object_class;

extern void* hi_object_new(hi_class* class_inf);

extern void* hi_object_retain(hi_object* object);

extern void* hi_object_release(hi_object* object);

extern void* hi_object_copying(hi_object* src, hi_object* dist);

extern hi_weakptr_t hi_object_weak_ptr_new(hi_object* object);
extern void* hi_object_weak_ptr_get(hi_weakptr_t ptr);

extern void* hi_object_delay_release(hi_object* object);

//Try to release all object not in pool.
extern void hi_object_pool_drain(hi_object* object);

extern hi_class* hi_class_from_object(hi_object* object);

#ifdef __cplusplus
}
#endif

#endif /* HI_OBJECT_H_ */
