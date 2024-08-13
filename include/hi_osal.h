#ifndef HI_OSAL_H_
#define HI_OSAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hi_types.h"

typedef struct
{
    void* (*memset_imp)(void* src, int value, hi_size_t size);
    void* (*memcpy_imp)(void* dst, const void* src, hi_size_t size);
    void* (*memmove_imp)(void* dst, const void* src, hi_size_t size);
    void* (*malloc_imp)(hi_size_t size);
    void* (*realloc_imp)(void* ptr, hi_size_t size);
    void (*free_imp)(void* ptr);
}hi_osal_memory_t;

typedef struct
{
    hi_time_t (*get_time_imp)(void);
}hi_osal_time_t;

typedef struct
{
    void (*sleep_imp)(hi_time_t ms);
}hi_osal_thread_t;

typedef struct
{
    int	(*printf_imp)(const char * __restrict, ...);
}hi_osal_io_t;

typedef struct
{
    hi_osal_io_t io;
    hi_osal_memory_t mem;
    hi_osal_time_t time;
    hi_osal_thread_t thread;
}hi_osal_t;

/**
 * @brief Used to define own hi_osal.
 * 
 */
extern hi_osal_t* hi_osal(void);

#ifdef __cplusplus
}
#endif

#endif /* HI_OSAL_H_ */
