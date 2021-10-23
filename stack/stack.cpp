#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h> 
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "stack.h"
#include "errors_and_logs.h"

//============================================================

int64_t get_hash_(char* base, unsigned int len, FUNC_FILE_LINE_PARAMS) {

    log_report();

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

//============================================================

static int stack_save_hash_(stack* stack, FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    #ifdef HASH

        int64_t hash = get_hash((char*)stack, sizeof(struct stack) - 2 * sizeof(int64_t));
        stack->base_hash = hash;

        if (stack->data != (elem_t*)(DEFAULT_PTR)
            && stack->data != (elem_t*)POISON_PTR
            && stack->data != NULL) {

            hash = get_hash((char*)stack->data,\
                                   stack->capacity 
                                 * stack->size_of_elem);
            stack->data_hash = hash;
        }

    #endif

    return 0;
}

//============================================================

int stack_data_hash_check_(stack* stack, FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    #ifdef HASH

        if (stack->data != NULL
            && stack->data != (elem_t*)DEFAULT_PTR
            && stack->data != (elem_t*)POISON_PTR) {

            int64_t hash = get_hash((char*)stack->data,
                stack->capacity * stack->size_of_elem);

            if (stack->data_hash == hash)
                return 1;
            else
                return 0;
        }
    #endif

    return 0;
}

//============================================================

int stack_base_hash_check_(stack* stack, FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    #ifdef HASH

        int64_t hash = get_hash((char*)stack, sizeof(struct stack)
                                            - 2 * sizeof(int64_t));

        if (stack->base_hash == hash)
            return 1;
        else
            return 0;
    #endif

    return 0;
}

//============================================================

static int stack_set_canaries_(stack* stack, FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    *(int64_t*)((char*)stack->data - sizeof(int64_t)) = CANARY_VAL;

    *(int64_t*)((char*)stack->data 
                     + stack->capacity * stack->size_of_elem) = CANARY_VAL;

    return 0;
}

//============================================================

int clear_mem_(void* base, int n, int size, FUNC_FILE_LINE_PARAMS) {

    log_report();

    if (base == NULL || n < 1 || size < 1) {

        set_global_error_code(CLEAR_MEM_CHECK_ERR);
        global_error_process();

        return 0;
    }

    memset(base, 0, n * size);

    return clear_mem_check(base, n, size);
}

//=============================================================

int clear_mem_check_(void* base, int n, int size, FUNC_FILE_LINE_PARAMS) {

    log_report();

    if (base == NULL || n < 0 || size < 0) {

        set_global_error_code(CLEAR_MEM_CHECK_ERR);
        global_error_process();

        return 0;
    }

    char* base_char = (char*)base;
    int char_n = n * size;

    for (int char_ct = 0; char_ct < char_n; char_ct++) {

        if (base_char[char_ct] != 0) { 

            return 0; 
        }
    }

    return 1;
}

//=============================================================

static int stack_free_data_check_(stack* stack, FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    if (stack->data == NULL
        || stack->data == (elem_t*)DEFAULT_PTR
        || stack->data == (elem_t*)POISON_PTR) {

        set_stack_error_code(STK_INV_DATA_PTR, stack);
        stack_error_process(stack);

        return -1;
    }

    return clear_mem_check(&stack->data[stack->count],
                            stack->capacity - stack->count, 
                            stack->size_of_elem);
}
//=============================================================

int stack_validator_(stack* stack, FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    int check_val = 0;

    #ifdef DEBUG

        #ifdef HASH

            if (stack_base_hash_check(stack) == 0) {

                set_stack_error_code(STK_BASE_HASH_ERR, stack);
                stack_error_process(stack);

                check_val++;
            }

            if (stack->data != NULL
                && stack->data != (elem_t*)DEFAULT_PTR
                && stack->data != (elem_t*)POISON_PTR
                && stack_data_hash_check(stack) == 0) {

                set_stack_error_code(STK_DATA_HASH_ERR, stack);
                stack_error_process(stack);

                check_val++;
            }

        #endif

        if (stack->self_ptr != stack) {

            set_stack_error_code(STK_INV_SELF_PTR, stack);
            stack_error_process(stack);

            check_val++;
        }

        #ifdef CANARIES

            if (stack->canary1 != CANARY_VAL) {

                set_stack_error_code(STK_CANARY1_ERR, stack);
                stack_error_process(stack);

                check_val++;
            }

            if (stack->canary2 != CANARY_VAL) {

                set_stack_error_code(STK_CANARY1_ERR, stack);
                stack_error_process(stack);

                check_val++;
            }

            if (stack->data != NULL
                && stack->data != (elem_t*)DEFAULT_PTR
                && stack->data != (elem_t*)POISON_PTR) {

                if (*(int64_t*)((char*)stack->data - sizeof(int64_t)) != CANARY_VAL) {

                    set_stack_error_code(STK_DATA_CANARY1_ERR, stack);
                    stack_error_process(stack);

                    check_val++;
                }

                if (*(int64_t*)((char*)stack->data
                    + stack->capacity * stack->size_of_elem) != CANARY_VAL) {

                    set_stack_error_code(STK_DATA_CANARY2_ERR, stack);
                    stack_error_process(stack);

                    check_val++;
                }
            }

        #endif

        if  (stack->origin.orig_file_name != NULL
             &&(clear_mem_check(stack->origin.orig_file_name, NAMEBUFSIZE, sizeof(char)) == 1
             || strncmp(stack->origin.orig_file_name, POISONSTR, NAMEBUFSIZE) == 0)) {

            set_stack_error_code(STK_INV_ORIG_FILE, stack);
            stack_error_process(stack);

            check_val++;
        }

        if  (stack->origin.orig_func_name != NULL
             && (clear_mem_check(stack->origin.orig_func_name, NAMEBUFSIZE, sizeof(char)) == 1
             || strncmp(stack->origin.orig_func_name, POISONSTR, NAMEBUFSIZE) == 0)) {

            set_stack_error_code(STK_INV_ORIG_FUNC, stack);
            stack_error_process(stack);

            check_val++;
        }

        if (stack->origin.orig_line == 0) {

            set_stack_error_code(STK_INV_ORIG_LINE, stack);
            stack_error_process(stack);

            check_val++;
        }

    #endif 

    if ((stack->capacity > 0 
        && stack->data == (elem_t*)DEFAULT_PTR)
        || stack->data == NULL
        || stack->data == (elem_t*)POISON_PTR) {

        set_stack_error_code(STK_INV_DATA_PTR, stack);
        stack_error_process(stack);

        check_val++;
    }

    if (strncmp("stack_push_", func_name, NAMEBUFSIZE) == 0
        && stack->capacity == MAXCAPACITY
        && stack->count == MAXCAPACITY) {

        set_stack_error_code(STK_OVRFLW, stack);
        stack_error_process(stack);

        stack_save_hash(stack);
        return -1;
    }

    if (strncmp("stack_pop_", func_name, NAMEBUFSIZE) == 0
        && stack->count == 0
        && stack->capacity == 0) {

        set_stack_error_code(STK_NO_ELEMS_PUSHED, stack);
        stack_error_process(stack);

        stack_save_hash(stack);
    }

    if (stack->size_of_elem != sizeof(elem_t)
        || stack->size_of_elem <= 0) {

        set_stack_error_code(STK_INV_SZ_ELEM, stack);
        stack_error_process(stack);

        check_val++;
    }

    if (stack->capacity < 0) {

        set_stack_error_code(STK_INV_CAPACITY, stack);
        stack_error_process(stack);

        check_val++;
    }

    if (stack->count < 0) {

        set_stack_error_code(STK_INV_ELEM_CT, stack);
        stack_error_process(stack);

        check_val++;
    }

    if (stack->capacity < stack->count) {

        set_stack_error_code(STK_CAPACITY_LESS_CT, stack);
        stack_error_process(stack);

        check_val++;
    }
    if (stack->size_of_elem > 0 
        && stack->count != 0 
        && stack->capacity != 0 
        && stack_free_data_check(stack) == 0) {

        set_stack_error_code(STK_INV_FREE_DATA, stack);
        stack_error_process(stack);

        check_val++;
    }

    if (check_val > 0) {

        #ifdef DEBUG
            stack_dump_(stack, FUNC_FILE_LINE_USE_ARGS);
        #endif

        return -1;
    }

    return 1;
}

//=============================================================

void* recalloc_(void* ptr, int number, int prev_number, int size_of_elem,
                                                    FUNC_FILE_LINE_PARAMS) {
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

        int value = clear_mem(base, offset, size_of_elem);
        if (value != 1) return NULL;
    }

    return new_ptr;
}

//==============================================================

static int stack_poison_check_(stack* stack, FUNC_FILE_LINE_PARAMS) {
    
    log_report();
    stack_ptr_check(stack);

    int check_value = 0;

    check_value += (stack->data  != (elem_t*)POISON_PTR);
    check_value += (stack->count        != POISON_VALUE);
    check_value += (stack->capacity     != POISON_VALUE);
    check_value += (stack->size_of_elem != POISON_VALUE);
    check_value += (stack->error_code   != POISON_VALUE);

    #ifdef DEBUG

        #ifdef HASH
        check_value += (stack->base_hash != POISON_VALUE);
        check_value += (stack->data_hash != POISON_VALUE);
        #endif

        #ifdef CANARIES
        check_value += (stack->canary1   != POISON_VALUE);
        check_value += (stack->canary2   != POISON_VALUE);
        #endif

        check_value += (strcmp(stack->origin.orig_file_name, POISONSTR));
        check_value += (strcmp(stack->origin.orig_func_name, POISONSTR));
        check_value += (stack->origin.orig_line         != POISON_VALUE);

    #endif

    if (check_value == 0) return 1;

    return 0;
}

//==============================================================

static int stack_empty_check_(stack* stack, FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    return clear_mem_check(stack, 1, sizeof(stack));
}

//==============================================================
#ifdef DEBUG

static int stack_set_origin_(stack* stack, FUNC_FILE_LINE_PARAMS) {

    stack_ptr_check(stack);

    stack->origin.orig_line = line;
    strncpy(stack->origin.orig_file_name, file_name, NAMEBUFSIZE);
    strncpy(stack->origin.orig_func_name, func_name, NAMEBUFSIZE);

    return 0;
}

#endif
//==============================================================

int stack_ctor_(stack* stack,  FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    if (stack_ctor_valid_check(stack) != 1) {

        set_stack_error_code(STK_CTOR_UNPREPARED, stack);
        stack_error_process(stack);

        return -1;
    }

    stack_preparing(stack);

    #ifdef DEBUG
        stack_set_origin_(stack, FUNC_FILE_LINE_USE_ARGS);
    #endif

    #ifdef HASH
        stack_save_hash(stack);
    #endif

    int ret_val = stack_validator(stack);
    if (ret_val == -1)
        return -1;

    return 0;
}

//==============================================================

int stack_set_size_(stack* stack, int new_size, FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    if (stack->count > new_size) {

        set_stack_error_code(STK_SZ_LESS_CT, stack);
        stack_error_process(stack);

        return -1;
    }

    if (new_size < 1) {

        set_stack_error_code(STK_INV_NEW_SIZE, stack);
        stack_error_process(stack);

        return -1;
    }

    int prev_capacity = stack->capacity;
    stack->capacity = new_size;

    if (stack->count == 0 && prev_capacity == 0) {

        #ifdef CANARIES
            elem_t* new_data_ptr = (elem_t*)calloc(new_size * stack->size_of_elem 
                                                   + 2 * sizeof(int64_t),
                                                         sizeof(char));
        #else 
            elem_t* new_data_ptr = (elem_t*)calloc(new_size, stack->size_of_elem);
        #endif

        if (new_data_ptr == NULL) {

            set_global_error_code(CANNOT_ALLOCATE_MEM);
            global_error_process();

            return -1;
        }


        else
            #ifdef CANARIES
                stack->data = (elem_t*)((char*)new_data_ptr + sizeof(int64_t));
                stack_set_canaries(stack);
            #else    
                stack->data = new_data_ptr;
            #endif

            #ifdef HASH
                stack_save_hash(stack);
            #endif
        }

    else {

        #ifdef CANARIES
            elem_t* new_data_ptr = (elem_t*)recalloc((char*)stack->data - sizeof(int64_t),
                                    new_size *  stack->size_of_elem + 2 * sizeof(int64_t),
                                    prev_capacity * stack->size_of_elem + sizeof(int64_t),
                                                                          sizeof(char));
        #else
            elem_t* new_data_ptr = (elem_t*)recalloc(stack->data, new_size,
                                       prev_capacity, stack->size_of_elem);
        #endif

        if (new_data_ptr == NULL) {

            set_global_error_code(CANNOT_ALLOCATE_MEM);
            global_error_process();

            return -1;
        }

        else {

            #ifdef CANARIES
                stack->data = (elem_t*)((char*)new_data_ptr + sizeof(int64_t));

                stack_set_canaries(stack);
            #else
                stack->data = new_data_ptr;
            #endif
        }
    }

    #ifdef HASH
        stack_save_hash(stack);
    #endif

    return 0;
}

//==============================================================

int stack_free_data_(stack* stack, FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    if (stack->data == (elem_t*)DEFAULT_PTR
        || stack->data == (elem_t*)POISON_PTR
        || stack->data == NULL) return -1;

    #ifdef CANARIES
        free((char*)stack->data - sizeof(int64_t));
    #else
        free(stack->data);
    #endif

    return 0;
}

//==============================================================

int stack_dtor_(stack* stack, FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    int ret_val = stack_validator(stack);
    if (ret_val == -1)
        return -1;

    stack_poisoning(stack);

    if (stack_poison_check(stack) == 0) {

        set_stack_error_code(STK_POISON_CHECK, stack);
        stack_error_process(stack);

        return -1;
    }
    
    return 0;
}

//==============================================================

int stack_poisoning_(stack* stack, FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    stack->capacity     = POISON_VALUE;
    stack->count        = POISON_VALUE;
    stack->size_of_elem = POISON_VALUE;

    stack->error_code   = POISON_VALUE;

    #ifdef DEBUG

        #ifdef HASH
            stack->base_hash = POISON_VALUE;
            stack->data_hash = POISON_VALUE;
        #endif    

        #ifdef CANARIES
            stack->canary1 = POISON_VALUE;
            stack->canary2 = POISON_VALUE;
        #endif

        strncpy(stack->origin.orig_file_name, POISONSTR, NAMEBUFSIZE);
        strncpy(stack->origin.orig_func_name, POISONSTR, NAMEBUFSIZE);
        stack->origin.orig_line = POISON_VALUE;

    #endif

    stack_free_data(stack);
    stack->data = (elem_t*)POISON_PTR;

    return 0;
}

//==============================================================

int stack_preparing_(stack* stack, FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    stack->data = (elem_t*)DEFAULT_PTR;

    stack->capacity = 0;
    stack->count = 0;
    stack->size_of_elem = sizeof(elem_t);

    stack->error_code = 0;

    #ifdef DEBUG

        #ifdef CANARIES
            stack->canary1 = CANARY_VAL;
            stack->canary2 = CANARY_VAL;
        #endif 

        stack->self_ptr = stack;

        if (strlen(stack->origin.orig_file_name) != 0) 
            clear_mem(stack->origin.orig_file_name, NAMEBUFSIZE, sizeof(char));

        if (strlen(stack->origin.orig_func_name) != 0) 
            clear_mem(stack->origin.orig_func_name, NAMEBUFSIZE, sizeof(char));

        stack->origin.orig_line = 0;

        #ifdef HASH
            stack_save_hash(stack);
        #endif

    #endif

    return 0;
}

//==============================================================

int stack_ctor_valid_check_(stack* stack, FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    if (stack_empty_check(stack) || stack_poison_check(stack)) return 1;

    else return 0;
}

//==============================================================

static int stack_resize_(stack* stack, int param, FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    if (stack->capacity == MAXCAPACITY && param == EXPAND) {

        set_stack_error_code(STK_OVRFLW, stack);
        stack_error_process(stack);

        return -1;
    }

    int prev_capacity = stack->capacity;

    switch (param) {

    case EXPAND: {

        stack->capacity *= 2; break;
    }

    case REDUCE: { 

        stack->capacity /= 2; break; 
    }

    default: {

        set_stack_error_code(STK_UNKNOWN_RESZ_PARAM, stack);
        stack_error_process(stack);

        return -1;
        }
    }

    if (stack->capacity > MAXCAPACITY) stack->capacity = MAXCAPACITY;

    #ifdef CANARIES
        elem_t* new_data_ptr = (elem_t*)recalloc((char*)stack->data - sizeof(int64_t),
                          stack->capacity * stack->size_of_elem + 2 * sizeof(int64_t), 
                                prev_capacity * stack->size_of_elem + sizeof(int64_t),
                                                                      sizeof(char));
    #else
        elem_t* new_data_ptr = (elem_t*)recalloc(stack->data,
                                                 stack->capacity, 
                                                 prev_capacity,
                                                 stack->size_of_elem);
    #endif

    if (new_data_ptr == NULL) {

        set_global_error_code(CANNOT_ALLOCATE_MEM);
        global_error_process();

        return -1;
    }

    else {

        #ifdef CANARIES

            stack->data = (elem_t*)((char*)new_data_ptr + sizeof(int64_t));
            stack_set_canaries(stack);

        #else
            stack->data = new_data_ptr;
        #endif

        #ifdef HASH
            stack_save_hash(stack);
        #endif
    }

    return 0;
}

//==============================================================

int stack_push_(stack* stack, elem_t value, FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    int ret_val = stack_validator(stack);
    if (ret_val == -1)
        return -1;

    if (stack->capacity == 0 && stack->count == 0) 
        stack_set_size(stack,STACK_STARTING_SIZE);

    if (stack->count + 1 > stack->capacity) {

        int stack_resize_ret = stack_resize(stack, EXPAND);
        if (stack_resize_ret == -1)
            return -1;
    }

    stack->data[stack->count++] = value;

    #ifdef HASH
        stack_save_hash(stack);
    #endif

    ret_val = stack_validator(stack);
    if (ret_val == -1)
        return -1;

    return 0;
}

//===============================================================

int stack_poped_elem_set_zero_(stack* stack, FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    clear_mem(stack->data + stack->count, 
              stack->size_of_elem, sizeof(char));

    if (stack->data[stack->count] == 0) return 1;

    return 0;
}

//===============================================================

elem_t stack_pop_(stack* stack, FUNC_FILE_LINE_PARAMS, int* err) {

    int err_val = 0;

    log_report();

    if (stack == NULL) {

        set_global_error_code(INV_STACK_PTR);
        global_error_process();

        if (err) *err = -1;
        return -1;
    }

    if (stack->capacity != 0
        && stack->count == 0) {

        set_stack_error_code(STK_UNDFLW, stack);
        stack_error_process(stack);

        stack_save_hash(stack);
        return -1;
    }

    int ret_val = stack_validator(stack);
    if (ret_val == -1) {

        if (err) *err = -1;
        return -1;
    }

    elem_t returning_value = stack->data[--stack->count];

    stack_poped_elem_set_zero(stack);

    if (stack->count < (stack->capacity / 4)
        && stack->capacity > STACK_STARTING_SIZE) 

        stack_resize(stack, REDUCE);

    #ifdef HASH
        stack_save_hash(stack);
    #endif

    ret_val = stack_validator(stack);
    if (ret_val == -1) {

        if (err) *err = -1;
        return -1;
    }

    return returning_value;
}

//===================================================================

#ifdef DEBUG

int stack_empty_orig_file_check(stack* stack) {

    //log_report();
    stack_ptr_check(stack);

    int value = 0;
    for (int i = 0; i < NAMEBUFSIZE; i++)
        value += stack->origin.orig_file_name[i];

    if (value == 0) return 1;

    return 0;
}

#endif

//===================================================================

#ifdef DEBUG

int stack_empty_orig_func_check(stack* stack) {

    //log_report();
    stack_ptr_check(stack);

    int value = 0;
    for (int i = 0; i < NAMEBUFSIZE; i++)
        value += stack->origin.orig_func_name[i];

    if (value == 0) return 1;

    return 0;
}

#endif

//===================================================================

int stack_out_(stack* stack, FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    stack_validator(stack);

    extern FILE* error_file;

    fprintf(error_file, "\n" "Stack <%p> Element type: " TYPE_NAME
                        "\n", stack);

    fprintf(error_file, "Size of stack element " "%lu" "\n",
                         sizeof(elem_t));

    fprintf(error_file, "Stack capacity: %d. Number of element in stack %d" "\n\n",
                         stack->capacity, stack->count);

    for (int count = 1; count <= stack->count; count++)

        fprintf(error_file, "[%d] " ELEM_SPEC "\n", count,
                            stack->data[count - 1]);

    fprintf(error_file, "\n\n");

    return 0;
}

//===================================================================

#ifdef DEBUG 

int stack_dump_(stack* stack, FUNC_FILE_LINE_PARAMS) {

    log_report();
    stack_ptr_check(stack);

    extern FILE* error_file;

    fprintf(error_file, "*************************************************\n\n");

    fprintf(error_file, "\n""Critical error occured during work with stack.""\n");
    fprintf(error_file, "\n""Function srack_dump has been called from %s, %s line %d""\n",
                                                                 FUNC_FILE_LINE_USE_ARGS);

    fprintf(error_file, "Stack address <%p>. Stack element type %s""\n\n", stack, TYPE_NAME);


    fprintf(error_file, "Stack origin conctruction file name: ");

    if (stack->origin.orig_file_name == NULL)
        fprintf(error_file, "ERROR: NULL POINTER\n");

    if (stack->origin.orig_file_name != NULL
        && stack_empty_orig_file_check(stack) == 1)
        fprintf(error_file, "ERROR: EMPTY ORIG FILE NAME\n");

    else fprintf(error_file, "%s\n", stack->origin.orig_file_name);


    fprintf(error_file, "Stack origin conctruction function name: ");

    if (stack->origin.orig_func_name == NULL)
        fprintf(error_file, "ERROR: NULL POINTER\n");

    if (stack->origin.orig_func_name != NULL
        && stack_empty_orig_func_check(stack) == 1)
        fprintf(error_file, "ERROR: EMPTY ORIG FUNCTION NAME\n");

    else fprintf(error_file, "%s\n", stack->origin.orig_func_name);


    fprintf(error_file, "Stack origin construction line: ");

    if (stack->origin.orig_line == 0)
        fprintf(error_file, "ERROR: Line number equals zero\n");
    else
        fprintf(error_file, "%d\n", stack->origin.orig_line);


    fprintf(error_file, "Size of element type is %lu\n", sizeof(elem_t));

    if (stack->size_of_elem != sizeof(elem_t))
        fprintf(error_file, "ERROR: Size of element type is not equal"
                            " size of stack element\n");

    if (stack->size_of_elem == 0)
        fprintf(error_file, "ERROR: Size of element equals zero\n");

    if (stack->size_of_elem != 0)
        fprintf(error_file, "Size of stack element: %d\n", stack->size_of_elem);

    #ifdef CANARIES
        fprintf(error_file, "Canary protection: ");

        if (stack->canary1 == CANARY_VAL)
            fprintf(error_file, "left is valid, ");
        else
            fprintf(error_file, "ERROR left canary protection failed, ");

        if (stack->canary2 == CANARY_VAL)
            fprintf(error_file, "right is valid\n\n");
        else
            fprintf(error_file, "ERROR right canary protection failed\n\n");

    #endif

    fprintf(error_file, "Stack capacity : %d. Number of elements in stack: %d\n",
        stack->capacity, stack->count);

    if (stack->capacity == 0)
        fprintf(error_file, "ERROR: Capacity equals zero\n");

    /*if (stack->count == 0 && stack->capacity != 0)
        fprintf(error_file, "ERROR: Size of element can not equal zero"
                            " until capacity is zero too\n");*/

    if (stack->capacity < 0)
        fprintf(error_file, "ERROR: Capacity is negative value\n");

    if (stack->count < 0)

        fprintf(error_file, "ERROR: Number of elements in stack is negative value\n");

    if (stack->count > stack->capacity)
        fprintf(error_file, "ERROR: Capacity cannot be lower than number"
                            " of elements in stack\n");

    
    if (stack->capacity >= 0
        && stack->size_of_elem == sizeof(elem_t)
        && stack->data != NULL
        && stack->data != (elem_t*)POISON_PTR
        && stack->data != (elem_t*)DEFAULT_PTR 
        && *(int64_t*)((char*)stack->data - sizeof(int64_t)) != CANARY_VAL)
        fprintf(error_file, "ERROR: left canary protection of stack->data has fallen.\n");

    else if (stack->capacity >= 0
        && stack->data != NULL
        && stack->data != (elem_t*)POISON_PTR
        && stack->data != (elem_t*)DEFAULT_PTR
        && stack->size_of_elem == sizeof(elem_t)
        && *(int64_t*)((char*)stack->data + stack->capacity * stack->size_of_elem) != CANARY_VAL)

        fprintf(error_file, "ERROR: right canary protection of stack->data has fallen.\n");

    else
        fprintf(error_file, "Canary protection works\n");

    fprintf(error_file, "Stack data pointer: ");

    if (stack->data == NULL)
        fprintf(error_file, "ERROR: NULL data pointer\n\n");

    else if (stack->data == (elem_t*)DEFAULT_PTR && stack->capacity != 0)
        fprintf(error_file, "ERROR: Data pointer is incorrect.\n "
                            "Can not equal DEFAULT_PTR after pushing or size setting");

    else if (stack->data == (elem_t*)POISON_PTR)
        fprintf(error_file, "ERROR: POISON_PTR for stack->data means that stack was destructed");

    else if (stack->data == (elem_t*)DEFAULT_PTR && stack->count == 0)
        fprintf(error_file, "Data pointer is DEFAUlT_PTR "
                            "Probably stack has not been used to push elements");

    else fprintf(error_file, "<%p>\n", stack->data);

    if (stack->data != NULL
        && stack->data != (elem_t*)POISON_PTR
        && stack->data != (elem_t*)DEFAULT_PTR
        && stack->count > 0 
        && stack->capacity > 0 
        && stack->capacity >= stack->count
        && clear_mem_check(&stack->data[stack->count], 
                            stack->capacity - stack->count + 1,
                            stack->size_of_elem)) {

        fprintf(error_file, "Elements of the stack: \n\n");

        for (int count = 0; count < stack->count; count++) {

            fprintf(error_file, "[%d]" ELEM_SPEC "\n", count, stack->data[count - 1]);
        }

        //if ((TYPE_NAME == "int" || TYPE_NAME == "integer"))
        //    for (int count = 0; count < stack->count; count++) {

        //        fprintf(error_file, "[%d] %5d\n", count, stack->data[count]);
        //    }

        //if ((TYPE_NAME == "double" || TYPE_NAME == "float") && stack->count != 0)
        //    for (int count = 0; count < stack->count; count++) {

        //        //fprintf(error_file, "[%d] %5g\n", count, stack->data[count]);
        //    }
    }

    if (stack->data != NULL
        && stack->data != (elem_t*)POISON_PTR
        && stack->data != (elem_t*)DEFAULT_PTR
        && !clear_mem_check(&stack->data[stack->count],
                            stack->capacity - stack->count + 1, 
                            stack->size_of_elem))

        fprintf(error_file, "ERROR: Stack free data is not empty\n");

    if (stack->count == 0 && stack->capacity == 0)
        fprintf(error_file, "Stack is empty\n");

    stack_error_process(stack);

    stack_free_data(stack);

    close_log_output();
    close_error_output();

    assert(0 && "CRITICAL ERROR: Stack dump has been called");
    return 0;
}
#endif


