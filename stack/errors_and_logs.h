#pragma once

#include <assert.h>
#include <stdint.h>

#include "../stack/stackconfig.h"
#include "stack.h"
#include "errors.h"
#include "../assembler/processor_general.h"

#define FUNC_FILE_LINE_PARAMS \
        const char* func_name, const char* file_name, int line

#define FUNC_FILE_LINE_GET_ARGS \
        __FUNCTION__, __FILE__, __LINE__

#define FUNC_FILE_LINE_USE_ARGS \
        func_name, file_name, line

#define stack_ptr_check(stack)                                       \
                                                                     \
        if (stack == NULL) {                                         \
            set_global_error_code(INV_STACK_PTR);                    \
            global_error_process();                                  \
            return -1;                                               \
        };

#ifdef DEBUG

    #define global_error_process() \
            global_error_process_(FUNC_FILE_LINE_GET_ARGS)

    #define stack_error_process(stack) \
            stack_error_process_(FUNC_FILE_LINE_GET_ARGS, stack) 

    #define stack_dump(stack) \
            stack_dump_(stack, FUNC_FILE_LINE_GET_ARGS)

    #define stack_set_canaries(stack) \
            stack_set_canaries_(stack, FUNC_FILE_LINE_GET_ARGS)

#else

    #define global_error_process() " "
    #define stack_error_process(stack)  " "
    #define stack_dump(stack) " " 

#endif 

#ifdef LOGS

    #define log_report() \
            log_report_(FUNC_FILE_LINE_USE_ARGS, __FUNCTION__)

    #define smpl_log_report() \
            smpl_log_report_(FUNC_FILE_LINE_GET_ARGS)

#else

    #define log_report() ""
    #define smpl_log_report() ""

#endif

#ifdef ASM_LOGS

    #define asm_log_report() \
            log_report_(FUNC_FILE_LINE_USE_ARGS, __FUNCTION__)

    #define asm_smpl_log_report() \
            smpl_log_report_(FUNC_FILE_LINE_GET_ARGS)

#else 

    #define asm_log_report() ""

    #define asm_smpl_log_report() ""

#endif

#ifdef PROC_LOGS

        #define proc_log_report() \
                log_report_(FUNC_FILE_LINE_USE_ARGS, __FUNCTION__)

        #define proc_smpl_log_report() \
                smpl_log_report_(FUNC_FILE_LINE_GET_ARGS)

#else 

#define asm_log_report() ""

#define asm_smpl_log_report() ""

#endif

#ifdef DISASM_LOGS

        #define disasm_log_report() \
                log_report_(FUNC_FILE_LINE_USE_ARGS, __FUNCTION__)

        #define disasm_smpl_log_report() \
                smpl_log_report_(FUNC_FILE_LINE_GET_ARGS)

#else 

        #define disasm_log_report() ""

        #define disasm_smpl_log_report() ""

#endif
//===================================================================


/// Log output
/// 
/// Sets up log output file
/// @param const char* filename name of the log_file
/// @return -1 if error occured and 0 if work ended successfully
int set_log_output(const char* filename);

int close_log_output(void);

/// Error output
/// 
/// Sets up error and syack_dump_ output file
/// @param const char* filename name of the standart error output file
/// @return -1 if error occured and 0 if work ended successfully
int set_error_output(const char* filename);

int close_error_output(void);

/// Log report function
/// 
/// Function works only in DEBUG mode
/// Prints a message in log_file that contains line, names of the function
/// and file of the place where current function was called from and the name
/// of the function cirrently performing 
/// @param const char* mother_func name of the functuion that is currenly working
/// @return -1 if error occured and 0 if work ended successfully
int log_report_(FUNC_FILE_LINE_PARAMS, const char* mother_func);

int log_report_parameters_check(FUNC_FILE_LINE_PARAMS);

int smpl_log_report_(FUNC_FILE_LINE_PARAMS);

//===========================================================================================

/// Sets up global error code
/// 
/// Sets up value in global variable global_error_code that is processing by
/// global_error_process();
/// @param int error_code code of the error that will be set up in global variable
void set_global_error_code(int error_code);

/// Checks global_error_code
/// 
/// Checks global_error_code global variable if it is 0 (no errors ocureed)
/// @return 1 if global_error_code is not zero and 0 if it is
int global_error_code_check(void);

/// Global error processing function
/// 
/// Calles global_error_code_check and checks returning value
/// Calles global_error_report if returning value is 1
/// @return 0 if work ends successfully
int global_error_process_(FUNC_FILE_LINE_PARAMS);

/// Global error report
/// 
/// Prints in error output file by FILE* error_file 
/// Prints code of global error,  file, function names and number of the line where 
/// global_error_process was called
/// @return 0 if work ends successfully and -1 if error occures
int global_error_report(FUNC_FILE_LINE_PARAMS);

//===========================================================================================

/// Sets up stack error code
/// 
/// Sets up value in error_code variable in stack structure
/// @param int error_code code of the error that will be set up in error_code variable
/// @return 0 if work ends successfully and -1 if error occures
int set_stack_error_code(int error_code, struct stack* stack);

/// Checks stack error code
/// 
/// Checks error_code variable in stack structure if it is 0 (no errors ocureed)
/// @param stack* stack - stack structure pointer 
/// @return 1 if stack error_code variable is not zero and 0 if it is
int stack_error_code_check(stack* stack);

/// Stack error processing function
/// 
/// Calles stack_error_code_check and checks returning value
/// Calles stack_error_report if returning value is 1
/// @param stack* stack - stack structure pointer
/// @return 0 if work ends successfully
int stack_error_process_(FUNC_FILE_LINE_PARAMS, stack* stack);

/// Stack error report
/// 
/// Prints in error output file by FILE* error_file 
/// Prints code of stack error,  file, function names and number of the line where 
/// global_error_process was called\
/// @param stack* stack - stack structure pointer 
/// @return 0 if work ends successfully and -1 if error occures
int stack_error_report(FUNC_FILE_LINE_PARAMS, stack* stack);

//===========================================================================================

/// Description of the error
/// 
/// Used to get const string that contains bried description of the error
/// @param error code
/// @return const char* description of the error
const char* get_error_descr(int error_code);

//===================================================================

#define $ if (err_val == -1) return -1;

//===================================================================

#define set_and_process_err(error_code) {                           \
                                                                    \
    set_global_error_code(error_code);                              \
    global_error_process();                                         \
                                                                    \
    return -1;                                                      \
    }                                                               

