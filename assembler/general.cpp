#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "general.h"

//===================================================================

int _clear_memory(void* base, int num, int size, LOG_PARAMS) {

    asm_log_report();

    if (base == NULL || num < 1 || size < 1) {

        set_global_error_code(CLEAR_MEM_CHECK_ERR);
        global_error_process();

        return 0;
    }

    memset(base, 0, num * size);

    return is_memory_clear(base, num, size);
}

//===================================================================

int _is_memory_clear(void* base, int num, int size, LOG_PARAMS) {

    asm_log_report();

    if (base == NULL || num < 1 || size < 1) {

        set_global_error_code(CLEAR_MEM_CHECK_ERR);
        global_error_process();

        return 0;
    }

    char* base_char = (char*)base;
    int char_n = num * size;

    for (int char_ct = 0; char_ct < char_n; char_ct++) {

        if (base_char[char_ct] != 0) return 0; 
    }

    return 1;
}

//===================================================================

void* _my_recalloc(void* ptr, int number, int prev_number, int size_of_elem,
                                                    LOG_PARAMS) {
    asm_log_report();

    if (ptr == NULL) {

        set_global_error_code(RECALLOC_INV_POINTER);
        global_error_process();

        return NULL;
    }

    int new_size = number * size_of_elem;
    void* new_ptr = realloc(ptr, new_size);

    if (new_ptr == NULL) {

        set_global_error_code(CANNOT_ALLOCATE_MEM);
        global_error_process();

        return NULL;
    }

    if (number > prev_number) {

        int offset = number - prev_number;
        char* base = (char*)new_ptr + size_of_elem * prev_number;

        int value = clear_memory(base, offset, size_of_elem);
        if (value != 1) 
            return NULL;
    }

    return new_ptr;
}

//===================================================================

void ___System_Sleep(float _Duration) {

    int64_t start = clock();
    while ((clock() - start) < int64_t(1000000 * _Duration));
}

//===================================================================

int64_t get_hash_(char* base, unsigned int len, LOG_PARAMS) {

    stack_log_report();

    assert(base);

    const unsigned int m = 0x5bd1e995;
    const unsigned int seed = 0;
    const int r = 24;

    unsigned int h = seed ^ len;

    const unsigned char* data = (const unsigned char*)base;
    unsigned int k = 0;

    while (len >= 4) {

        k = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;

    }

    switch (len) {
        case 3:
            h ^= data[2] << 16;
        case 2:
            h ^= data[1] << 8;
        case 1:
            h ^= data[0];
            h *= m;
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}
