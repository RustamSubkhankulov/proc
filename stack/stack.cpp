#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h> 
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "stack.h"
#include "errors_and_logs.h"

//===================================================================

static int stack_save_hash_(stack* stack, LOG_PARAMS) {

    stack_log_report();
    stack_ptr_check(stack);

    #ifdef HASH

        int base_hash_size = sizeof(struct stack) - 2 * sizeof(int64_t);

        int64_t hash = get_hash((char*)stack, base_hash_size);
        stack->base_hash = hash;

        if (stack->data != (elem_t*)(DEFAULT_PTR)
            && stack->data != (elem_t*)POISON_PTR
            && stack->data != NULL) {

            int data_hash_size = stack->capacity 
                               * stack->size_of_elem;

            hash = get_hash((char*)stack->data, data_hash_size);
            stack->data_hash = hash;
        }

    #endif

    return 0;
}

//===================================================================

#ifdef HASH

int stack_data_hash_check_(stack* stack, LOG_PARAMS) {

    stack_log_report();
    stack_ptr_check(stack);

        if (stack->data != NULL
            && stack->data != (elem_t*)DEFAULT_PTR
            && stack->data != (elem_t*)POISON_PTR) {

            int size = stack->capacity 
                     * stack->size_of_elem;

            int64_t hash = get_hash((char*)stack->data, size);

            if (stack->data_hash == hash)
                return 1;
            else
                return 0;
        }

    return 0;
}

#endif

//===================================================================

#ifdef HASH

int stack_base_hash_check_(stack* stack, LOG_PARAMS) {

    stack_log_report();
    stack_ptr_check(stack);

    int hash_size = sizeof(struct stack) - 2 * sizeof(int64_t);

    int64_t hash = get_hash((char*)stack, hash_size);

    if (stack->base_hash == hash)
        return 1;
    else
        return 0;
}

#endif

//===================================================================

static int stack_set_canaries_(stack* stack, LOG_PARAMS) {

    stack_log_report();
    stack_ptr_check(stack);

    int64_t* canary_ptr = (int64_t*)stack->data - 1;

    *canary_ptr = CANARY_VAL;

    int canary_offset = stack->capacity * sizeof(elem_t);

    canary_ptr = (int64_t*)((char*)stack->data + canary_offset);

    *canary_ptr = CANARY_VAL;

    return 0;
}

//===================================================================

int clear_mem_(void* base, int n, int size, LOG_PARAMS) {

    stack_log_report();

    if (base == NULL || n < 1 || size < 1) 
        set_and_process_err(CLEAR_MEM_CHECK_ERR);

    memset(base, 0, n * size);

    return clear_mem_check(base, n, size);
}

//===================================================================

int clear_mem_check_(void* base, int n, int size, LOG_PARAMS) {

    stack_log_report();

    if (base == NULL || n < 0 || size < 0) 
         set_and_process_err(CLEAR_MEM_CHECK_ERR);       

    char* base_char = (char*)base;
    int char_n = n * size;

    for (int char_ct = 0; char_ct < char_n; char_ct++) {

        if (base_char[char_ct] != 0) 
            return 0; 
    }

    return 1;
}

//===================================================================

static int stack_free_data_check_(stack* stack, LOG_PARAMS) {

    stack_log_report();
    stack_ptr_check(stack);

    if (stack->data == NULL
        || stack->data == (elem_t*)DEFAULT_PTR
        || stack->data == (elem_t*)POISON_PTR) {

        set_stack_error_code(STK_INV_DATA_PTR, stack);
        stack_error_process(stack);

        return -1;
    }

    int size = stack->size_of_elem;
    int number = stack->capacity - stack->count;
    void* pointer = &stack->data[stack->count];

    return clear_mem_check(pointer, number, size);
}

//===================================================================

#ifdef CANARIES

int _stack_canary_validation(stack* stack, LOG_PARAMS) {

    int check_val = 0;

    if (stack->canary1 != CANARY_VAL) {

        stack_error(STK_CANARY1_ERR, stack);
        check_val++;
    }

    if (stack->canary2 != CANARY_VAL) {

        stack_error(STK_CANARY1_ERR, stack);
        check_val++;
    }

    if (stack->data != NULL
        && stack->data != (elem_t*)DEFAULT_PTR
        && stack->data != (elem_t*)POISON_PTR) {

        int64_t* canary_ptr =(int64_t*)stack->data - 1;

        if (*canary_ptr != CANARY_VAL) {

            stack_error(STK_DATA_CANARY1_ERR, stack);
            check_val++;
        }

        int offset = stack->capacity * sizeof(elem_t);

        canary_ptr =(int64_t*)((char*)stack->data + offset);

        if (*canary_ptr != CANARY_VAL) {

            stack_error(STK_DATA_CANARY2_ERR, stack);
            check_val++;
        }
    }

    if (check_val != 0)
        return 1;
    else
        return 0;
}

#endif

//===================================================================

int stack_validator_(stack* stack, LOG_PARAMS) {

    stack_log_report();
    stack_ptr_check(stack);

    int check_val = 0;

    #ifdef HASH

        if (stack_base_hash_check(stack) == 0) {

            stack_error(STK_BASE_HASH_ERR, stack);
            check_val++;
        }

        if (stack->data != NULL
            && stack->data != (elem_t*)DEFAULT_PTR
            && stack->data != (elem_t*)POISON_PTR
            && stack_data_hash_check(stack) == 0) {

            stack_error(STK_DATA_HASH_ERR, stack);
            check_val++;
        }

    #endif

    if (stack->self_ptr != stack) {

        stack_error(STK_INV_SELF_PTR, stack);
        check_val++;
    }

    #ifdef CANARIES

        check_val += stack_canary_validation(stack);   
        
    #endif

    if ((stack->capacity > 0 
        && stack->data == (elem_t*)DEFAULT_PTR)
        || stack->data == NULL
        || stack->data == (elem_t*)POISON_PTR) {

        stack_error(STK_INV_DATA_PTR, stack);
        check_val++;
    }

    if (strncmp("stack_push_", func_name, NAMEBUFSIZE) == 0
        && stack->capacity == MAXCAPACITY
        && stack->count == MAXCAPACITY) {

        stack_error(STK_OVRFLW, stack);
        stack_save_hash(stack);

        return -1;
    }

    if (strncmp("stack_pop_", func_name, NAMEBUFSIZE) == 0
        && stack->count == 0
        && stack->capacity == 0) {

        stack_error(STK_NO_ELEMS_PUSHED, stack);
        stack_save_hash(stack);
    }

    if (stack->size_of_elem != sizeof(elem_t)
        || stack->size_of_elem <= 0) {

        stack_error(STK_INV_SZ_ELEM, stack);
        check_val++;
    }

    if (stack->capacity < 0) {

        stack_error(STK_INV_CAPACITY, stack);
        check_val++;
    }

    if (stack->count < 0) {

        stack_error(STK_INV_ELEM_CT, stack);
        check_val++;
    }

    if (stack->capacity < stack->count) {

        stack_error(STK_CAPACITY_LESS_CT, stack);
        check_val++;
    }
    if (stack->size_of_elem > 0 
        && stack->count != 0 
        && stack->capacity != 0 
        && stack_free_data_check(stack) == 0) {

        stack_error(STK_INV_FREE_DATA, stack);
        check_val++;
    }

    if (check_val > 0) {

        #ifdef DEBUG
            stack_dump_(stack, LOGS_ARGS_USE);
        #endif

        return -1;
    }

    return 1;
}

//===================================================================

void* recalloc_(void* ptr, int number, int prev_number, 
                          int size_of_elem, LOG_PARAMS) {
    stack_log_report();

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
        
        if (value != 1) 
            return NULL;
    }

    return new_ptr;
}

//===================================================================

static int stack_poison_check_(stack* stack, LOG_PARAMS) {
    
    stack_log_report();
    stack_ptr_check(stack);

    int check_value = 0;

    check_value += (stack->data  != (elem_t*)POISON_PTR);
    check_value += (stack->count        != POISON_VALUE);
    check_value += (stack->capacity     != POISON_VALUE);
    check_value += (stack->size_of_elem != POISON_VALUE);
    check_value += (stack->error_code   != POISON_VALUE);

    #ifdef HASH

        check_value += (stack->base_hash != POISON_VALUE);
        check_value += (stack->data_hash != POISON_VALUE);
    
    #endif

    #ifdef CANARIES
        
        check_value += (stack->canary1   != POISON_VALUE);
        check_value += (stack->canary2   != POISON_VALUE);
        
    #endif

    if (check_value == 0) return 1;

    return 0;
}

//===================================================================

static int stack_empty_check_(stack* stack, LOG_PARAMS) {

    stack_log_report();
    stack_ptr_check(stack);

    return clear_mem_check(stack, 1, sizeof(stack));
}

//===================================================================
#ifdef DEBUG

static int stack_set_origin_(stack* stack, LOG_PARAMS) {

    stack_ptr_check(stack);

    stack->origin.orig_line = line;
    strncpy(stack->origin.orig_file_name, file_name, NAMEBUFSIZE);
    strncpy(stack->origin.orig_func_name, func_name, NAMEBUFSIZE);

    return 0;
}

#endif
//===================================================================

int stack_ctor_(stack* stack,  LOG_PARAMS) {

    stack_log_report();
    stack_ptr_check(stack);

    if (stack_ctor_valid_check(stack) != 1) {

        stack_error(STK_CTOR_UNPREPARED, stack);
        return -1;
    }

    stack_preparing(stack);

    #ifdef DEBUG
        stack_set_origin_(stack, LOGS_ARGS_USE);
    #endif

    #ifdef HASH
        stack_save_hash(stack);
    #endif

    #ifdef DEBUG 

        int ret_val = stack_validator(stack);
        
        if (ret_val == -1)
            return -1;

    #endif 

    return 0;
}

//===================================================================

int stack_set_size_(stack* stack, int new_size, LOG_PARAMS) {

    stack_log_report();
    stack_ptr_check(stack);

    if (stack->count > new_size) {

        stack_error(STK_SZ_LESS_CT, stack);
        return -1;
    }

    if (new_size < 1) {

        stack_error(STK_INV_NEW_SIZE, stack);
        return -1;
    }

    int prev_capacity = stack->capacity;
    stack->capacity = new_size;

    if (stack->count == 0 && prev_capacity == 0) {

        #ifdef CANARIES

            int size = new_size * stack->size_of_elem 
                               + 2 * sizeof(int64_t);

            elem_t* new_data_ptr = (elem_t*)calloc(size, 
                                                   sizeof(char));

        #else 

            elem_t* new_data_ptr = (elem_t*)calloc(new_size, 
                                        stack->size_of_elem);
        
        #endif

        if (new_data_ptr == NULL) 
            set_and_process_err(CANNOT_ALLOCATE_MEM)

        else {

            #ifdef CANARIES

                char* data_ptr = (char*)new_data_ptr 
                                  + sizeof(int64_t);

                stack->data = (elem_t*)data_ptr;
                stack_set_canaries(stack);

            #else    
                stack->data = new_data_ptr;
            #endif

            #ifdef HASH
                stack_save_hash(stack);
            #endif
        }    
    }

    else {

        #ifdef CANARIES

            int64_t* data_ptr = (int64_t*)stack->data - 1;

            int size      = new_size * sizeof(elem_t)  + 2 * sizeof(int64_t);
            int prev_size = prev_capacity * sizeof(elem_t) + sizeof(int64_t);

            elem_t* new_data_ptr = (elem_t*)recalloc(data_ptr, size, 
                                                     prev_size, sizeof(char));
        #else
            elem_t* new_data_ptr = (elem_t*)recalloc(stack->data, new_size,
                                            prev_capacity, sizeof(elem_t));
        #endif

        if (new_data_ptr == NULL) 
            set_and_process_err(CANNOT_ALLOCATE_MEM);

        #ifdef CANARIES

            stack->data = (elem_t*)((char*)new_data_ptr + sizeof(int64_t));
            stack_set_canaries(stack);

        #else
            stack->data = new_data_ptr;
        #endif
    }

    #ifdef HASH
        stack_save_hash(stack);
    #endif

    return 0;
}

//==============================================================

int stack_free_data_(stack* stack, LOG_PARAMS) {

    stack_log_report();
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

int stack_dtor_(stack* stack, LOG_PARAMS) {

    stack_log_report();
    stack_ptr_check(stack);

    #ifdef DEBUG

        int ret_val = stack_validator(stack);
        if (ret_val == -1)
            return -1;

    #endif 

    stack_poisoning(stack);

    if (stack_poison_check(stack) == 0) {

        stack_error(STK_POISON_CHECK, stack);
        return -1;
    }
    
    return 0;
}

//==============================================================

int stack_poisoning_(stack* stack, LOG_PARAMS) {

    stack_log_report();
    stack_ptr_check(stack);

    stack->capacity     = POISON_VALUE;
    stack->count        = POISON_VALUE;
    stack->size_of_elem = POISON_VALUE;

    stack->error_code   = POISON_VALUE;

    #ifdef HASH
        stack->base_hash = POISON_VALUE;
        stack->data_hash = POISON_VALUE;
    #endif    

    #ifdef CANARIES
        stack->canary1 = POISON_VALUE;
        stack->canary2 = POISON_VALUE;
    #endif

    stack_free_data(stack);
    stack->data = (elem_t*)POISON_PTR;

    return 0;
}

//==============================================================

int stack_preparing_(stack* stack, LOG_PARAMS) {

    stack_log_report();
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

        #ifdef HASH
            stack_save_hash(stack);
        #endif

    #endif

    return 0;
}

//==============================================================

int stack_ctor_valid_check_(stack* stack, LOG_PARAMS) {

    stack_log_report();
    stack_ptr_check(stack);

    if (stack_empty_check(stack) || stack_poison_check(stack)) 
        return 1;
    else 
        return 0;
}

//==============================================================

static int stack_resize_(stack* stack, int param, LOG_PARAMS) {

    stack_log_report();
    stack_ptr_check(stack);

    if (stack->capacity == MAXCAPACITY && param == EXPAND) {

        stack_error(STK_OVRFLW, stack);
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

            stack_error(STK_UNKNOWN_RESZ_PARAM, stack);
            return -1;
            }
    }

    if (stack->capacity > MAXCAPACITY) 
        stack->capacity = MAXCAPACITY;

    #ifdef CANARIES

        int64_t* data_ptr = (int64_t*)stack->data - 1;

        int size =    stack->capacity * sizeof(elem_t) + 2 * sizeof(int64_t);
        int prev_size = prev_capacity * sizeof(elem_t) + sizeof(int64_t);

        elem_t* new_data_ptr = (elem_t*)recalloc(data_ptr, size, 
                                       prev_size, sizeof(char));

    #else

        elem_t* new_data_ptr = (elem_t*)recalloc(stack->data,
                                                 stack->capacity, 
                                                 prev_capacity,
                                                 stack->size_of_elem);
    #endif

    if (new_data_ptr == NULL) 
        set_and_process_err(CANNOT_ALLOCATE_MEM)

    else {

        #ifdef CANARIES

            int64_t* data_ptr = (int64_t*)new_data_ptr + 1;

            stack->data = (elem_t*)(data_ptr);
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

int stack_push_(stack* stack, elem_t value, LOG_PARAMS) {

    stack_log_report();
    stack_ptr_check(stack);

    #ifdef DEBUG

        int ret_val = stack_validator(stack);
        if (ret_val == -1)
            return -1;

    #endif 

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

    #ifdef DEBUG

        ret_val = stack_validator(stack);
        if (ret_val == -1)
            return -1;

    #endif 

    return 0;
}

//===============================================================

int stack_poped_elem_set_zero_(stack* stack, LOG_PARAMS) {

    stack_log_report();
    stack_ptr_check(stack);

    clear_mem(stack->data + stack->count, 
              stack->size_of_elem, sizeof(char));

    if (stack->data[stack->count] == 0) return 1;

    return 0;
}

//===============================================================

elem_t stack_pop_(stack* stack, LOG_PARAMS, int* err) {

    int err_val = 0;

    stack_log_report();

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

    #ifdef DEBUG

        int ret_val = stack_validator(stack);
        if (ret_val == -1) {

            if (err) *err = -1;
            return -1;
        }

    #endif 

    elem_t returning_value = stack->data[--stack->count];

    stack_poped_elem_set_zero(stack);

    if (stack->count < (stack->capacity / 4)
        && stack->capacity > STACK_STARTING_SIZE) 

        stack_resize(stack, REDUCE);

    #ifdef HASH
        stack_save_hash(stack);
    #endif

    #ifdef DEBUG

        ret_val = stack_validator(stack);
        if (ret_val == -1) {

            if (err) *err = -1;
            return -1;
        }

    #endif 

    return returning_value;
}

//===================================================================

#ifdef DEBUG

int stack_empty_orig_file_check(stack* stack, LOG_PARAMS) {

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

int stack_empty_orig_func_check(stack* stack, LOG_PARAMS) {

    //stack_log_report();
    stack_ptr_check(stack);

    int value = 0;
    for (int i = 0; i < NAMEBUFSIZE; i++)
        value += stack->origin.orig_func_name[i];

    if (value == 0) return 1;

    return 0;
}

#endif

//===================================================================

int stack_out_(stack* stack, LOG_PARAMS) {

    stack_log_report();
    stack_ptr_check(stack);

    stack_validator(stack);

    fprintf(logs_file, "\n" "Stack <%p> Element type: " TYPE_NAME
                        "\n", stack);

    fprintf(logs_file, "Size of stack element " "%lu" "\n",
                         sizeof(elem_t));

    fprintf(logs_file, "Stack capacity: %d. Number of element in stack %d" "\n\n",
                         stack->capacity, stack->count);

    for (int count = 1; count <= stack->count; count++)

        fprintf(logs_file, "[%d] DEC " ELEM_SPEC "  HEX %x" "\n", count,
                                                 stack->data[count - 1], 
                                           (int)stack->data[count - 1]);

    fprintf(logs_file, "\n\n");

    return 0;
}

//===================================================================

#ifdef DEBUG 

int stack_dump_(stack* stack, LOG_PARAMS) {

    stack_log_report();
    stack_ptr_check(stack);

    fprintf(logs_file, "\n""Critical error occured"
                       " during work with stack.""\n");

    fprintf(logs_file, "\n""Function srack_dump has"
                       " been called from %s, %s line %d""\n",
                                  func_name, file_name, line);

    fprintf(logs_file, "Stack address <%p>. Stack:"
                       " element type %s""\n\n", stack, TYPE_NAME);

    fprintf(logs_file, "Stack origin conctruction file name: ");

    if (stack->origin.orig_file_name == NULL)
        fprintf(logs_file, "ERROR: NULL POINTER\n");

    if (stack->origin.orig_file_name != NULL
        && stack_empty_orig_file_check(stack, LOGS_ARGS_USE) == 1)

        fprintf(logs_file, "ERROR: EMPTY ORIG FILE NAME\n");

    else
        fprintf(logs_file, "%s\n", stack->origin.orig_file_name);


    fprintf(logs_file, "Stack origin conctruction function name: ");

    if (stack->origin.orig_func_name == NULL)
        fprintf(logs_file, "ERROR: NULL POINTER\n");

    if (stack->origin.orig_func_name != NULL
        && stack_empty_orig_func_check(stack, LOGS_ARGS_USE) == 1)

        fprintf(logs_file, "ERROR: EMPTY ORIG FUNCTION NAME\n");

    else 

        fprintf(logs_file, "%s\n", stack->origin.orig_func_name);


    fprintf(logs_file, "Stack origin construction line: ");

    if (stack->origin.orig_line == 0)
        fprintf(logs_file, "ERROR: Line number equals zero\n");
    else
        fprintf(logs_file, "%d\n", stack->origin.orig_line);


    fprintf(logs_file, "Size of element type is %lu\n", sizeof(elem_t));

    if (stack->size_of_elem != sizeof(elem_t))
        fprintf(logs_file, "ERROR: Size of element type is not equal"
                                         " size of stack element\n");

    if (stack->size_of_elem == 0)
        fprintf(logs_file, "ERROR: Size of element equals zero\n");

    if (stack->size_of_elem != 0)
        fprintf(logs_file, "Size of stack element: %d\n", 
                                    stack->size_of_elem);

    #ifdef CANARIES

        fprintf(logs_file, "Canary protection: ");

        if (stack->canary1 == CANARY_VAL)
            fprintf(logs_file, "left is valid, ");
        else
            fprintf(logs_file, "ERROR left canary protection failed, ");

        if (stack->canary2 == CANARY_VAL)
            fprintf(logs_file, "right is valid\n\n");
        else
            fprintf(logs_file, "ERROR right canary protection failed\n\n");

    #endif

    fprintf(logs_file, "Stack capacity : %d. Number of"
                            " elements in stack: %d\n",
                        stack->capacity, stack->count);

    if (stack->capacity == 0)
        fprintf(logs_file, "ERROR: Capacity equals zero\n");

    if (stack->capacity < 0)
        fprintf(logs_file, "ERROR: Capacity is negative value\n");

    if (stack->count < 0)
        fprintf(logs_file, "ERROR: Number of elements in"
                            " stack is negative value\n");

    if (stack->count > stack->capacity)
        fprintf(logs_file, "ERROR: Capacity cannot be lower than number"
                                             " of elements in stack\n");
    #ifdef CANARIES

        int64_t* canary_ptr1 = (int64_t*)stack->data - 1;
        int64_t* canary_ptr2 = (int64_t*)((char*)stack->data 
                                               + stack->capacity * sizeof(elem_t));
    
        if (stack->capacity >= 0
            && stack->data != NULL
            && stack->data != (elem_t*)POISON_PTR
            && stack->data != (elem_t*)DEFAULT_PTR 
            && *canary_ptr1 != CANARY_VAL)

            fprintf(logs_file, "ERROR: left canary protection "
                               "of stack->data has fallen.\n");

        else if (stack->capacity >= 0
            && stack->data != NULL
            && stack->data != (elem_t*)POISON_PTR
            && stack->data != (elem_t*)DEFAULT_PTR
            && *canary_ptr2 != CANARY_VAL)

            fprintf(logs_file, "ERROR: right canary protection of"
                                    " stack->data has fallen.\n");

        else
            fprintf(logs_file, "Canary protection works\n");

    #endif

    fprintf(logs_file, "Stack data pointer: ");

    if (stack->data == NULL)
        fprintf(logs_file, "ERROR: NULL data pointer\n\n");

    else if (stack->data == (elem_t*)DEFAULT_PTR && stack->capacity != 0)

        fprintf(logs_file, "ERROR: Data pointer is incorrect.\n "
                               "Can not equal DEFAULT_PTR after "
                                      "pushing or size setting");

    else if (stack->data == (elem_t*)POISON_PTR)
        fprintf(logs_file, "ERROR: POISON_PTR for stack->data"
                           " means that stack was destructed");

    else if (stack->data == (elem_t*)DEFAULT_PTR 
          && stack->count == 0)

        fprintf(logs_file, "Data pointer is DEFAUlT_PTR "
                            "Probably stack has not been"
                                " used to push elements");

    else fprintf(logs_file, "<%p>\n", stack->data);

    if (stack->data != NULL
        && stack->data != (elem_t*)POISON_PTR
        && stack->data != (elem_t*)DEFAULT_PTR
        && stack->count > 0 
        && stack->capacity > 0 
        && stack->capacity >= stack->count
        && clear_mem_check(&stack->data[stack->count], 
                            stack->capacity - stack->count + 1,
                            stack->size_of_elem)) {

        fprintf(logs_file, "Elements of the stack: \n\n");

        for (int count = 0; count < stack->count; count++) {

            fprintf(logs_file, "[%d]" ELEM_SPEC "\n", 
                      count, stack->data[count - 1]);
        }
    }

    if (stack->data != NULL
        && stack->data != (elem_t*)POISON_PTR
        && stack->data != (elem_t*)DEFAULT_PTR
        && !clear_mem_check(&stack->data[stack->count],
                             stack->capacity - stack->count + 1, 
                             stack->size_of_elem))

        fprintf(logs_file, "ERROR: Stack free data is not empty\n");

    if (stack->count == 0 && stack->capacity == 0)
        fprintf(logs_file, "Stack is empty\n");

    stack_error_process(stack);

    stack_free_data(stack);

    fflush(logs_file);

    assert(0 && "CRITICAL ERROR: Stack dump has been called");
    return 0;
}
#endif


