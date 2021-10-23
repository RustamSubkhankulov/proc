#pragma once

#include "../assembler/assembler.h"

struct ProcStruct {

    long code_file_size;
    const unsigned char* code_array;
    const unsigned char* ip;

    int command_ct;
    int elem_t_args_ct;
    int register_args_ct;
    int ram_using_commands_number;

    elem_t regist[REGISTER_SIZE];

    char ram[RAM_SIZE];

    struct Header header;

    struct stack Stack;
};

typedef struct ProcStruct procstruct;

//===================================================================

#define procstruct_ptr_check(ptr) {                                 \
                                                                    \
    if (ptr == NULL)                                                \
        set_and_process_err(INV_PROCSTRUCT_PTR)                     \
}


#define procstruct_ip_check(procstruct) {                           \
                                                                    \
    if (procstruct->ip - procstruct->code_array                     \
        > procstruct->code_file_size)                               \
                                                                    \
    set_and_process_err(INV_INSTR_PTR)                              \
}

//===================================================================

#define init_procstruct(procstruct, fp) \
       _init_procstruct(procstruct, fp, FUNC_FILE_LINE_GET_ARGS)

#define dtor_procstruct(procstruct) \
       _dtor_procstruct(procstruct, FUNC_FILE_LINE_GET_ARGS)

//===================================================================

#define read_from_file(fp, size) \
       _read_from_file(fp, size,  FUNC_FILE_LINE_GET_ARGS)

#define header_check(procstruct) \
       _header_check(procstruct, FUNC_FILE_LINE_GET_ARGS)

//===================================================================

#define proc_perform(procstruct) \
       _proc_perform(procstruct, FUNC_FILE_LINE_GET_ARGS)

#define proc_execute_command(procstruct) \
       _proc_execute_command(procstruct, FUNC_FILE_LINE_GET_ARGS)

//===================================================================

#define PROC_ADD(procstruct, oper_code) \
       _PROC_ADD(procstruct, FUNC_FILE_LINE_GET_ARGS)

#define PROC_SUB(procstruct, oper_code) \
       _PROC_SUB(procstruct, FUNC_FILE_LINE_GET_ARGS)

#define PROC_MUL(procstruct, oper_code) \
       _PROC_MUL(procstruct, FUNC_FILE_LINE_GET_ARGS)

#define PROC_DIV(procstruct, oper_code) \
       _PROC_DIV(procstruct, FUNC_FILE_LINE_GET_ARGS)

#define PROC_PUSH(procstruct, oper_code) \
       _PROC_PUSH(procstruct, oper_code,  FUNC_FILE_LINE_GET_ARGS)

#define PROC_OUT(procstruct, oper_code) \
       _PROC_OUT(procstruct, FUNC_FILE_LINE_GET_ARGS)

#define PROC_POP(procstruct, oper_code) \
       _PROC_POP(procstruct, oper_code, FUNC_FILE_LINE_GET_ARGS)

#define PROC_IN(procstruct, oper_code) \
       _PROC_IN(procstruct, FUNC_FILE_LINE_GET_ARGS)

#define PROC_INIT(procstruct, oper_code) \
       _PROC_INIT(procstruct, FUNC_FILE_LINE_GET_ARGS)

#define PROC_HLT(procstruct, oper_code) \
       _PROC_HLT(procstruct, FUNC_FILE_LINE_GET_ARGS)

#define PROC_DUMP(procstruct, oper_code) \
       _PROC_DUMP(procstruct, FUNC_FILE_LINE_GET_ARGS)
//===================================================================

#define open_proc_output(filename) \
       _open_proc_output(filename, FUNC_FILE_LINE_GET_ARGS)

#define close_proc_output() \
       _close_proc_output(FUNC_FILE_LINE_GET_ARGS)

#define open_proc_input(filename) \
       _open_proc_input(filename, FUNC_FILE_LINE_GET_ARGS)

#define close_proc_input() \
       _close_proc_input(FUNC_FILE_LINE_GET_ARGS)

//===================================================================

#define proc_dump(procstruct) \
       _PROC_DUMP(procstruct, FUNC_FILE_LINE_GET_ARGS)

#define proc_final_check(procstruct) \
       _proc_final_check(procstruct, FUNC_FILE_LINE_GET_ARGS)

//===================================================================

int _PROC_DUMP(procstruct* procstruct, FUNC_FILE_LINE_PARAMS);

int _proc_final_check(procstruct* procstruct, FUNC_FILE_LINE_PARAMS);

//===================================================================

int _proc_perform(procstruct* procstruct, FUNC_FILE_LINE_PARAMS);

int _proc_execute_command(procstruct* procstruct, FUNC_FILE_LINE_PARAMS);

//===================================================================

int _init_procstruct(procstruct* procstruct, FILE* fp, FUNC_FILE_LINE_PARAMS);

int _dtor_procstruct(procstruct* procstruct, FUNC_FILE_LINE_PARAMS);

//===================================================================

const unsigned char* _read_from_file(FILE* fp, long* size, FUNC_FILE_LINE_PARAMS);

int _header_check(procstruct* procstruct, FUNC_FILE_LINE_PARAMS);

//===================================================================

int _PROC_PUSH(procstruct* procstruct, unsigned char oper_code, FUNC_FILE_LINE_PARAMS);

int _PROC_POP (procstruct* procstruct, unsigned char oper_code, FUNC_FILE_LINE_PARAMS);

int _PROC_ADD (procstruct* procstruct, FUNC_FILE_LINE_PARAMS);

int _PROC_SUB (procstruct* procstruct, FUNC_FILE_LINE_PARAMS);

int _PROC_MUL (procstruct* procstruct, FUNC_FILE_LINE_PARAMS);

int _PROC_DIV (procstruct* procstruct, FUNC_FILE_LINE_PARAMS);

int _PROC_OUT (procstruct* procstruct, FUNC_FILE_LINE_PARAMS);

int _PROC_IN  (procstruct* procstruct, FUNC_FILE_LINE_PARAMS);

int _PROC_INIT(procstruct* procstruct, FUNC_FILE_LINE_PARAMS);

int _PROC_HLT (procstruct* procstruct, FUNC_FILE_LINE_PARAMS);

//===================================================================

int _open_proc_output(const char* filename, FUNC_FILE_LINE_PARAMS);

int _close_proc_output(FUNC_FILE_LINE_PARAMS);

int _open_proc_input(const char* filename, FUNC_FILE_LINE_PARAMS);

int _close_proc_input(FUNC_FILE_LINE_PARAMS);
