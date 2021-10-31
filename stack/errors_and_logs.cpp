#define _CRT_SECURE_NO_WARNINGS

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "errors_and_logs.h"
#include "../stack/errors.h"
#include "../assembler/processor_general.h"

/// Global variable for error code
/// 
/// Global int variable used for global error code
/// that are not related to stack
static int global_error_code = 0;

//===================================================

FILE* open_log_file(const char* filename) {

    if (filename == NULL)
        return NULL;

    FILE* temp_ptr = fopen(filename, "wb");

    if (temp_ptr == NULL)
        return NULL;

    else 
        return temp_ptr;
    
}

//===================================================

int close_log_file(FILE* logs_file) {

    if (logs_file == NULL)
        return -1;

    int fclose_ret = fclose(logs_file);

    if (fclose_ret == EOF)
        return -1;

    return 0;
}

//===================================================

void set_global_error_code(int error_code) {

    extern int global_error_code;
    global_error_code = error_code;
}

//====================================================

int global_error_process_(LOG_PARAMS) {

    if (global_error_code_check())
        global_error_report(LOGS_ARGS_USE);

    return 0;
}

//====================================================

int stack_error_process_(LOG_PARAMS, stack* stack) {

    stack_ptr_check(stack);

    if (stack_error_code_check(stack, LOGS_ARGS_USE))
        stack_error_report(LOGS_ARGS_USE, stack);

    return 0;
}

//====================================================

int global_error_code_check(void) {

    extern int global_error_code;

    return (global_error_code == 0) ?  0: 1;
}

//====================================================

int stack_error_code_check(stack* stack, LOG_PARAMS) {

    stack_ptr_check(stack);

    return(stack->error_code == 0) ? 0 : 1;
}

//====================================================

int _set_stack_error_code(int error_code, 
                         stack* stack, LOG_PARAMS) {

    extern int global_error_code;

    stack_ptr_check(stack);

    stack->error_code = error_code;
    return 0;
}

//================================================

int log_report_parameters_check(LOG_PARAMS) {

    if (line <= 0 
    || logs_file == NULL
    || file_name == NULL 
    || func_name == NULL) 
    
        set_and_process_err(LOG_REPORT_PARAMS_ERR);

    return 1;
}

//================================================

int log_report_(LOG_PARAMS, const char* mother_func) {

    if (log_report_parameters_check(LOGS_ARGS_USE) == 0)
        return -1;

    int value = fprintf(logs_file, "Funtion: %s\n Called from: function: %s, file: %s."
                                      " Current line: %d.\n\n", mother_func, func_name, 
                                                                      file_name, line);

    if (value < 0) 
        set_and_process_err(FILE_OUTPUT_ERROR);

    return 0;
}

//================================================

int smpl_log_report_(LOG_PARAMS) {

    if (log_report_parameters_check(LOGS_ARGS_USE) == 0)
        return -1;

    int value = fprintf(logs_file, "Function: %s, file: %s, line: %d \n \n",
                                                func_name, file_name, line);

    if (value < 0) 
        set_and_process_err(FILE_OUTPUT_ERROR);

    return 0;
}

//================================================

int global_error_report(LOG_PARAMS) {

    extern FILE* log_output;
    extern int global_error_code;

    fprintf(stderr, "\n Oops, something went wrong." 
                    " Go see logs to find out. \n");
    
    fprintf(logs_file, "Global_error-code is %d\n", 
                                global_error_code);

    int value = fprintf(logs_file, "An error occured.\n File: %s, function: "
                                                "%s, line %d.""\n",file_name,
                                                                   func_name,
                                                                       line);

    value += fprintf(logs_file, "%s\n\n", 
                     get_error_descr(global_error_code));

    if (value < 0) 
        set_and_process_err(FILE_OUTPUT_ERROR);

    global_error_code = 0;
    return 0;
}

//================================================

int stack_error_report(LOG_PARAMS, stack* stack) {

    stack_ptr_check(stack);

    fprintf(logs_file, "Stack error code is %d\n", stack->error_code);
    
    int value = fprintf(logs_file, "An error occured during work with stack.\n"
                                        " File: %s, function: %s, line %d.""\n",  
                                                    file_name, func_name, line);

    value += fprintf(logs_file, "%s\n\n", 
                     get_error_descr(stack->error_code));

    if (value < 0) 
        set_and_process_err(FILE_OUTPUT_ERROR);

    return 0;
}

//=================================================

#define DEF_ERR(code, name, string)                 \
                                                    \
    case code : {                                   \
                                                    \
        return #string;                             \
                                                    \
        break;                                      \
    }                                               \

//=================================================

const char* get_error_descr(int error_code) {

    switch (error_code) {

    #include "../text_files/errors.txt"
    
    default:
        return NULL;
    }
}

#undef DEF_ERR