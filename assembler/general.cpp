#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "general.h"

//===================================================================

int _clear_memory(void* base, int num, int size, LOG_PARAMS) {

    log_report();

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

    log_report();

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
    log_report();

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

void ___System_Sleep(int _Duration) {

    int64_t start = clock();
    while ((clock() - start) < 1000000 * _Duration);
}

//===================================================================

void hack_pentagon(void) {

    for (int ct = 1; ct <= 10; ct++) {

        printf("\n Hacking Pentagon... Progress: ");
        printf("%d%%  *", ct * 10);
        
        int i = 1;
        for (; i <= ct * 10; i ++)
            printf("%c", '-');

        while (i++ <= 100)
            printf(" ");
        
        printf("*");

        ___System_Sleep(5);
    }
    printf("\nPentagon successfully hacked!\n");
}
