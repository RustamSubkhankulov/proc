#pragma once

#include "../stack/errors_and_logs.h"

//===================================================================

#define my_recalloc(ptr, number, prev_number, size_of_elem) \
       _my_recalloc(ptr, number, prev_number, size_of_elem, \
                                   LOG_ARGS)

#define clear_memory(base, num, size) \
       _clear_memory(base, num, size, LOG_ARGS)

#define is_memory_clear(base, num, size) \
       _is_memory_clear(base, num, size, LOG_ARGS)

//===================================================================

void* _my_recalloc(void* ptr, int number, int prev_number, int size_of_elem,
                                                    LOG_PARAMS);

int _clear_memory(void* base, int num, int size, LOG_PARAMS);

int _is_memory_clear(void* base, int num, int size, LOG_PARAMS);

void hack_pentagon(void);

void ___System_Sleep(int _Duration);