#pragma once

#include "errors_and_logs.h"

typedef struct stack stack;

struct stack_origin {

    char orig_file_name[NAMEBUFSIZE];
    char orig_func_name[NAMEBUFSIZE];
    int orig_line;
};

#ifdef DEBUG

    struct stack {

        #ifdef CANARIES
            int64_t canary1;
        #endif 

        elem_t* data;
        int count;
        int capacity;
        int size_of_elem;

        int error_code;

        stack* self_ptr;
        struct stack_origin origin;

        #ifdef CANARIES
            int64_t canary2;
        #endif 

        #ifdef HASH
            int64_t base_hash;
            int64_t data_hash;
        #endif
    };
#else
    struct stack {

        elem_t* data;
        int count;
        int capacity;
        int size_of_elem;

        int error_code;
    };
#endif

//===================================================================

#define LOG_PARAMS \
        const char* func_name, const char* file_name, int line, FILE* logs_file

#define LOG_ARGS \
        __FUNCTION__, __FILE__, __LINE__, logs_file

#define LOGS_ARGS_USE \
        func_name, file_name, line, logs_file

//===================================================================

#ifdef DEBUG
    #define stack_validator(stack)\
            stack_validator_(stack, LOG_ARGS)
#else
    #define stack_validator(stack) ""
#endif

//===================================================================

#define stack_save_hash(stack) \
        stack_save_hash_(stack, LOG_ARGS)

#define stack_ctor(stack) \
        stack_ctor_(stack, LOG_ARGS)

#define stack_dtor(stack) \
        stack_dtor_(stack, LOG_ARGS)

#define stack_pop(stack, err) \
        stack_pop_(stack, LOG_ARGS, err)

#define stack_push(stack, value) \
        stack_push_(stack, value, LOG_ARGS)

//===================================================================

#define stack_empty_check(stack) \
        stack_empty_check_(stack, LOG_ARGS)

#define stack_poison_check(stack) \
        stack_poison_check_(stack, LOG_ARGS)

#define stack_ctor_valid_check(stack) \
        stack_ctor_valid_check_(stack, LOG_ARGS)

#define stack_poped_elem_set_zero(stack) \
        stack_poped_elem_set_zero_(stack, LOG_ARGS)

#define stack_poisoning(stack) \
        stack_poisoning_(stack, LOG_ARGS)

#define stack_preparing(stack) \
        stack_preparing_(stack, LOG_ARGS)

#define stack_resize(stack, param) \
        stack_resize_(stack, param, LOG_ARGS)

#define stack_set_size(stack, size) \
        stack_set_size_(stack, size, LOG_ARGS)

#define stack_set_origin(stack) \
        stack_set_origin_(stack, LOG_ARGS)

#define stack_free_data(stack) \
        stack_free_data_(stack, LOG_ARGS)

//===================================================================

#define clear_mem_check(base, n, size) \
        clear_mem_check_(base, n, size, LOG_ARGS)

#define clear_mem(base, n, size) \
        clear_mem_(base, n, size, LOG_ARGS)

#define recalloc(ptr, number, prev_number, size_of_elem)  \
        recalloc_(ptr, number, prev_number, size_of_elem, \
                                 LOG_ARGS)

#define stack_free_data_check(stack) \
        stack_free_data_check_(stack, LOG_ARGS)

#define stack_set_canaries(stack) \
        stack_set_canaries_(stack, LOG_ARGS)

//===================================================================

#define stack_save_hash(stack) \
        stack_save_hash_(stack, LOG_ARGS)

#define stack_base_hash_check(stack) \
        stack_base_hash_check_(stack, LOG_ARGS)

#define stack_data_hash_check(stack) \
        stack_data_hash_check_(stack, LOG_ARGS)

#define get_hash(base, len) \
        get_hash_(base, len, LOG_ARGS)

//===================================================================

#define stack_out(stack) \
        stack_out_(stack, LOG_ARGS)

//===================================================================

static int stack_poison_check_(stack* stack, LOG_PARAMS);

static int stack_empty_check_(stack* stack, LOG_PARAMS);

static int stack_ctor_valid_check_(stack* stack, LOG_PARAMS);

//===================================================================

/// Stack constructor function
///
/// Calles stack_ctor_valid_check_ to check if stack structure
/// is prepared for constructing
/// If returning value of stack_ctor_check is 1 calles stack_preparing_();
/// Calles function that sets origin file and function names and line in DEBUG mode
/// Updates hash if hash protection is on
/// Calles stack_validator_() in the end to check preparing for the work
/// @return -1 if error occured and 0 if work ended successfully
int stack_ctor_(stack* stack, LOG_PARAMS);

/// Stack destructor function
///
/// Calles log_report_(), stack_ptr_check, stack_validator_()
/// Calles stack_poisoning_() function 
/// Calles stack_poison_check to check if poisoning ended successfully
/// @return 0 if work ende successfully and -1 if error occured
int stack_dtor_(stack* stack, LOG_PARAMS);

/// Pushes element to the stack
///
/// Calles log_report_(), stack_ptr_check() and stack_validator_();
/// Calles stack_set_size_ with STARTING SIZE defined constacnt value
/// when first element is pushed to the stack and stack_resize_()
/// if stack needs to be extended
/// Updates hash in HASH protection mode
/// Calles stack_validator_ at the end
int stack_push_(stack* stack, elem_t value, LOG_PARAMS);

elem_t stack_pop_(stack* stack, LOG_PARAMS, int* err = NULL);

//elem_t stack_summ_(stack* stack, LOG_PARAMS, int* err = NULL);
//

//===================================================================

static int stack_poped_elem_set_zero_(stack* stack, LOG_PARAMS);

static int stack_poisoning_(stack* stack, LOG_PARAMS);

static int stack_preparing_(stack* stack, LOG_PARAMS);

//===================================================================

static int stack_resize_(stack* stack, int param, LOG_PARAMS);

int stack_set_size_(stack* stack, int new_size, LOG_PARAMS);

//===================================================================

int stack_validator_(stack* stack, LOG_PARAMS);

int stack_out_(stack* stack, LOG_PARAMS);

int stack_dump_(stack* stack, LOG_PARAMS);

static int stack_free_data_check_(stack* stack, LOG_PARAMS);

//===================================================================

static int stack_set_origin_(stack* stack, LOG_PARAMS);

static int stack_empty_orig_file_check(stack* stack, LOG_PARAMS);

static int stack_empty_orig_func_check(stack* stack, LOG_PARAMS);

static int stack_set_canaries_(stack* stack, LOG_PARAMS);

//===================================================================

/// Saves hash in stack
///
/// Saves hash of the stack structure and stack data in stack
/// base_hash and data_hash after changes in stack
static int stack_save_hash_(stack* stack, LOG_PARAMS);

static int64_t get_hash_(char* base, unsigned int len, LOG_PARAMS);

static int stack_data_hash_check_(stack* stack, LOG_PARAMS);

static int stack_base_hash_check_(stack* stack, LOG_PARAMS);

//===================================================================

void* recalloc_(void* ptr, int number, int prev_number, int size_of_elem,
                                                  LOG_PARAMS);

int clear_mem_check_(void* base, int n, int size, LOG_PARAMS);

int clear_mem_(void* base, int n, int size, LOG_PARAMS);

//===================================================================