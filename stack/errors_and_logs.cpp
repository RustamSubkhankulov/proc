#define _CRT_SECURE_NO_WARNINGS

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "stackconfig.h"
#include "errors_and_logs.h"
#include "../stack/errors.h"

/// Global variable for error code
/// 
/// Global int variable used for global error code
/// that are not related to stack
static int global_error_code = 0;

//===================================================

void set_global_error_code(int error_code) {

    extern int global_error_code;
    global_error_code = error_code;
}

//====================================================

int global_error_process_(FUNC_FILE_LINE_PARAMS) {

    if (global_error_code_check())
        global_error_report(FUNC_FILE_LINE_USE_ARGS);

    return 0;
}

//====================================================

int stack_error_process_(FUNC_FILE_LINE_PARAMS, stack* stack) {

    stack_ptr_check(stack);

    if (stack_error_code_check(stack))
        stack_error_report(FUNC_FILE_LINE_USE_ARGS, stack);

    return 0;
}

//====================================================

int global_error_code_check(void) {

    extern int global_error_code;

    return (global_error_code == 0) ?  0: 1;
}

//====================================================

int stack_error_code_check(stack* stack) {

    stack_ptr_check(stack);

    return(stack->error_code == 0) ? 0 : 1;
}

//====================================================

int set_stack_error_code(int error_code, stack* stack) {

    extern int global_error_code;

    stack_ptr_check(stack);

    stack->error_code = error_code;
    return 0;
}
//====================================================

int set_error_output(const char* filename) {

    extern int global_error_code;
    extern FILE* error_file;

    if (filename == NULL) {

        set_global_error_code(INV_FILE_NAME);
        global_error_process();

        return -1;
    }

    else {

        FILE* error_file_temp = fopen(filename, "wb");

        if (error_file_temp == NULL) {

            set_global_error_code(FOPEN_ERROR);
            global_error_process();

            return -1;
        }
        else {

            error_file = error_file_temp;
        }
    }

    return 0;
}

//====================================================

int close_error_output(void) {

    extern FILE* error_file;

    if (error_file != stderr) {

        int value = fclose(error_file);

        if (value == EOF) {

            set_global_error_code(FCLOSE_ERROR);
            global_error_process();

            return -1;
        }
    }
        
    return 0;
}

//====================================================

int set_log_output(const char* filename) {

    extern int global_error_code;
    extern FILE* log_file;

    if (filename == NULL) {

        set_global_error_code(INV_FILE_NAME);
        global_error_process();

        return -1;
    }

    else {

        FILE* log_file_temp = fopen(filename, "wb");

        if (log_file_temp == NULL) {

            set_global_error_code(FOPEN_ERROR);
            global_error_process();

            return -1;
        }
        else {

            log_file = log_file_temp;
        }
    }

    return 0;
}

//====================================================

int close_log_output(void) {

    extern FILE* log_file;

    if (log_file != stderr) {

        int value = fclose(log_file);

        if (value == EOF) {

            set_global_error_code(FCLOSE_ERROR);
            global_error_process();

            return -1;
        }
    }

    return 0;
}
//================================================

int log_report_parameters_check(FUNC_FILE_LINE_PARAMS) {

    if (line <= 0 || file_name == NULL || func_name == NULL) {

        set_global_error_code(LOG_REPORT_PARAMS_ERR);
        global_error_process();

        return 0;
    }

    return 1;
}

//================================================

int log_report_(FUNC_FILE_LINE_PARAMS, const char* mother_func) {

    if (log_report_parameters_check(FUNC_FILE_LINE_USE_ARGS) == 0)
        return -1;

    extern FILE* log_file;

    int value = fprintf(log_file, "Funtion: %s\n Called from: function: %s, file: %s."
                                  " Current line: %d.\n\n", mother_func, FUNC_FILE_LINE_USE_ARGS);

    if (value < 0) {

        set_global_error_code(FILE_OUTPUT_ERROR);
        global_error_process();

        return -1;
    }

    return 0;
}

//================================================

int smpl_log_report_(FUNC_FILE_LINE_PARAMS) {

    if (log_report_parameters_check(FUNC_FILE_LINE_USE_ARGS) == 0)
        return -1;

    extern FILE* log_gile;

    int value = fprintf(log_file, "Function: %s, file: %s, line: %d \n \n",
                                                  FUNC_FILE_LINE_USE_ARGS);

    if (value < 0) {

        set_global_error_code(FILE_OUTPUT_ERROR);
        global_error_process();

        return -1;
    }

    return 0;
}

//================================================

int global_error_report(FUNC_FILE_LINE_PARAMS) {

    extern FILE* error_file;
    extern int global_error_code;

    //
    fprintf(error_file, "Global_error-code is %d\n", global_error_code);
    //

    int value = fprintf(error_file, "An error occured.\n File: %s, function: %s, line %d.""\n",
                                                                    file_name,
                                                                    func_name,
                                                                        line);
    value += fprintf(error_file, "%s\n\n", get_error_descr(global_error_code));

    if (value < 0) {
            
        set_global_error_code(FILE_OUTPUT_ERROR);
        global_error_process();

        return -1;
    }

    global_error_code = 0;
    return 0;
}

//================================================

int stack_error_report(FUNC_FILE_LINE_PARAMS, stack* stack) {

    stack_ptr_check(stack);

    extern FILE* error_file;

    fprintf(error_file, "Stack error code is %d\n", stack->error_code);
    
    int value = fprintf(error_file, "An error occured during work with stack.\n File: %s,"
                        " function: %s, line %d.""\n",  file_name, func_name, line);

    value += fprintf(error_file, "%s\n\n", get_error_descr(stack->error_code));

    if (value < 0) {

        set_global_error_code(FILE_OUTPUT_ERROR);
        global_error_process();

        return -1;
    }

    return 0;
}

//=================================================

const char* get_error_descr(int error_code) {

    switch (error_code) {

    case STK_INVALSIZE:
        return STK_INVALSIZE_;

    case STK_NO_MEM:
        return STK_NO_MEM_;

    case STK_OVRFLW:
        return STK_OVRFLW_;

    case STK_UNDFLW:
        return STK_UNDFLW_;

    case STK_NO_ELEMS_PUSHED:
        return STK_NO_ELEMS_PUSHED_;

    case STK_INV_NEW_SIZE:
        return STK_INV_NEW_SIZE_;

    case STK_CTOR_UNPREPARED:
        return STK_CTOR_UNPREPARED_;

    case STK_SZ_LESS_CT:
        return STK_SZ_LESS_CT_;

    case STK_UNKNOWN_RESZ_PARAM:
        return STK_UNKNOWN_RESZ_PARAM_;

    case STK_INV_SZ_ELEM:
        return STK_INV_SZ_ELEM_;

    case STK_INV_CAPACITY:
        return STK_INV_CAPACITY_;

    case STK_INV_ELEM_CT:
        return STK_INV_ELEM_CT_;

    case STK_CAPACITY_LESS_CT:
        return STK_CAPACITY_LESS_CT_;

    case STK_INV_FREE_DATA:
        return STK_INV_FREE_DATA_;

    case STK_INV_DATA_PTR:
        return STK_INV_DATA_PTR_;

    case STK_POISON_CHECK:
        return STK_POISON_CHECK_;

    case STK_EMPTY_CHECK:
        return STK_EMPTY_CHECK_;

    case STK_CANARY1_ERR:
        return STK_CANARY1_ERR_;

    case STK_CANARY2_ERR:
        return STK_CANARY2_ERR_;

    case STK_DATA_CANARY1_ERR:
        return STK_DATA_CANARY1_ERR_;

    case STK_DATA_CANARY2_ERR:
        return STK_DATA_CANARY2_ERR_;

    case STK_INV_ORIG_FUNC:
        return STK_INV_ORIG_FUNC_;

    case STK_INV_ORIG_FILE:
        return STK_INV_ORIG_FILE_;

    case STK_INV_ORIG_LINE:
        return STK_INV_ORIG_LINE_;

    case STK_INV_SELF_PTR:
        return STK_INV_SELF_PTR_;

    case INV_FILE_NAME:
        return INV_FILE_NAME_;

    case FOPEN_ERROR:
        return FOPEN_ERROR_;

    case FILE_OUTPUT_ERROR:
        return FILE_OUTPUT_ERROR_;

    case INV_ERROR_CODE:
        return INV_ERROR_CODE_;

    case INV_STACK_PTR:
        return INV_STACK_PTR_;

    case RECALLOC_INV_NUM_OF_ELEMS:
        return RECALLOC_INV_NUM_OF_ELEMS_;

    case RECALLOC_INV_POINTER:
        return RECALLOC_INV_POINTER_;

    case RECALLOC_INV_SIZE_OF_ELEM:
        return RECALLOC_INV_SIZE_OF_ELEM_;

    case CANNOT_ALLOCATE_MEM:
        return CANNOT_ALLOCATE_MEM_;

    case STK_BASE_HASH_ERR:
        return STK_BASE_HASH_ERR_;

    case STK_DATA_HASH_ERR:
        return STK_DATA_HASH_ERR_;

    case CLEAR_MEM_CHECK_ERR:
        return CLEAR_MEM_CHECK_ERR_;

    case TEXT_PROCESSING_ERR:
        return TEXT_PROCESSING_ERR_;

    case FCLOSE_ERROR:
        return FCLOSE_ERROR_;

    case INV_TEXT_PTR:
        return INV_TEXT_PTR_;

    case TEXT_NO_STRINGS:
        return TEXT_NO_STRINGS_;

    case NULL_STRING_TEXT:
        return NULL_STRING_TEXT_;

    case STACK_CALC_DIV_ZERO:
        return STACK_CALC_DIV_ZERO_;

    case INV_FILE_PTR:
        return INV_FILE_PTR_;

    case INV_HDR_PTR:
        return INV_HDR_PTR_;

    case FWRITE_ERR:
        return FWRITE_ERR_;

    case INV_INSTRUCTION_STR:
        return INV_INSTRUCTION_STR_;

    case CODE_FILE_SIZE_ERR:
        return CODE_FILE_SIZE_ERR_;

    default:
        return NULL;
    }
}