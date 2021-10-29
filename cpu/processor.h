#pragma once

#include "../assembler/assembler.h"

struct ProcStruct {

    long code_file_size;
    const unsigned char* code_array;
    const unsigned char* ip;

    int command_ct;

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

//===================================================================

#define get_destination_ip                                            \
    do                                                                \
    {                                                                 \
       procstruct->ip++;                                              \
       procstruct_ip_check(procstruct);                               \
       procstruct_ptr_check(procstruct);                              \
                                                                      \
       dest_ip = *(int*)procstruct->ip;                               \
        printf("\n\n ip is %d \n\n", dest_ip);                                                              \
    } while(0);

//===================================================================

#define get_two_args_from_stack                                       \
    do                                                                \
    {                                                                 \
       first_val = stack_pop(&procstruct->Stack, &err);               \
       if (err == -1)                                                 \
           return -1;                                                 \
                                                                      \
       second_val = stack_pop(&procstruct->Stack, &err);              \
       if (err == -1)                                                 \
           return -1;                                                 \
                                                                      \
    } while(0);                                                       

//===================================================================

#define get_ram_number                                                \
    do                                                                \
    {                                                                 \
       char reg_number = *procstruct->ip - 1;                         \
                                                                      \
        procstruct->ip++;                                             \
        procstruct_ip_check(procstruct);                              \
                                                                      \
        int value = *(int*)procstruct->ip;                            \
                                                                      \
        procstruct->ip += sizeof(int);                                \
        procstruct_ip_check(procstruct);                              \
                                                                      \
        ram_number = procstruct->regist[reg_number] + value;          \
        if (ram_number < 0 || ram_number > RAM_SIZE)                  \
            set_and_process_err(INV_RAM_NUMBER);                      \
                                                                      \
    } while (0);

//===================================================================

#define procstruct_ip_check(procstruct) {                           \
                                                                    \
    if (procstruct->ip - procstruct->code_array                     \
        > procstruct->code_file_size)                               \
                                                                    \
    set_and_process_err(INV_INSTR_PTR)                              \
}

//===================================================================

#define init_procstruct(procstruct, fp) \
       _init_procstruct(procstruct, fp, LOG_ARGS)

#define dtor_procstruct(procstruct) \
       _dtor_procstruct(procstruct, LOG_ARGS)

//===================================================================

#define read_from_file(fp, size) \
       _read_from_file(fp, size,  LOG_ARGS)

#define header_check(procstruct) \
       _header_check(procstruct, LOG_ARGS)

//===================================================================

#define proc_perform(procstruct) \
       _proc_perform(procstruct, LOG_ARGS)

#define proc_execute_command(procstruct) \
       _proc_execute_command(procstruct, LOG_ARGS)

//===================================================================

#define PROC_ADD(procstruct, oper_code) \
       _PROC_ADD(procstruct, LOG_ARGS)

#define PROC_SUB(procstruct, oper_code) \
       _PROC_SUB(procstruct, LOG_ARGS)

#define PROC_MUL(procstruct, oper_code) \
       _PROC_MUL(procstruct, LOG_ARGS)

#define PROC_DIV(procstruct, oper_code) \
       _PROC_DIV(procstruct, LOG_ARGS)

#define PROC_PUSH(procstruct, oper_code) \
       _PROC_PUSH(procstruct, oper_code,  LOG_ARGS)

#define PROC_OUT(procstruct, oper_code) \
       _PROC_OUT(procstruct, LOG_ARGS)

#define PROC_POP(procstruct, oper_code) \
       _PROC_POP(procstruct, oper_code, LOG_ARGS)

#define PROC_IN(procstruct, oper_code) \
       _PROC_IN(procstruct, LOG_ARGS)

#define PROC_INIT(procstruct, oper_code) \
       _PROC_INIT(procstruct, LOG_ARGS)

#define PROC_HLT(procstruct, oper_code) \
       _PROC_HLT(procstruct, LOG_ARGS)

#define PROC_DUMP(procstruct, oper_code) \
       _PROC_DUMP(procstruct, LOG_ARGS)

#define PROC_RET(procstruct, oper_code) \
       _PROC_RET(procstruct, LOG_ARGS)

//===================================================================

#define PROC_JMP(procstruct) \
       _PROC_JMP(procstruct, LOG_ARGS)

#define PROC_JA(procstruct) \
       _PROC_JA(procstruct, LOG_ARGS)

#define PROC_JAE(procstruct) \
       _PROC_JAE(procstruct, LOG_ARGS)

#define PROC_JB(procstruct) \
       _PROC_JB(procstruct, LOG_ARGS)

#define PROC_JBE(procstruct) \
       _PROC_JBE(procstruct, LOG_ARGS)

#define PROC_JE(procstruct) \
       _PROC_JE(procstruct, LOG_ARGS)

#define PROC_JNE(procstruct) \
       _PROC_JNE(procstruct, LOG_ARGS)

#define PROC_JF(procstruct) \
       _PROC_JF(procstruct, LOG_ARGS)

#define PROC_CALL(procstruct) \
       _PROC_CALL(procstruct, LOG_ARGS)

//===================================================================

#define open_proc_output(filename) \
       _open_proc_output(filename, LOG_ARGS)

#define close_proc_output() \
       _close_proc_output(LOG_ARGS)

#define open_proc_input(filename) \
       _open_proc_input(filename, LOG_ARGS)

#define close_proc_input() \
       _close_proc_input(LOG_ARGS)

//===================================================================

#define proc_dump(procstruct) \
       _PROC_DUMP(procstruct, LOG_ARGS)

#define proc_final_check(procstruct) \
       _proc_final_check(procstruct, LOG_ARGS)

//===================================================================

int _proc_final_check(procstruct* procstruct, LOG_PARAMS);

//===================================================================

int _proc_perform(procstruct* procstruct, LOG_PARAMS);

int _proc_execute_command(procstruct* procstruct, LOG_PARAMS);

//===================================================================

int _init_procstruct(procstruct* procstruct, FILE* fp, LOG_PARAMS);

int _dtor_procstruct(procstruct* procstruct, LOG_PARAMS);

//===================================================================

const unsigned char* _read_from_file(FILE* fp, long* size, LOG_PARAMS);

int _header_check(procstruct* procstruct, LOG_PARAMS);

//===================================================================

int _PROC_PUSH(procstruct* procstruct, unsigned char oper_code, LOG_PARAMS);

int _PROC_POP (procstruct* procstruct, unsigned char oper_code, LOG_PARAMS);

int _PROC_ADD (procstruct* procstruct, LOG_PARAMS);

int _PROC_SUB (procstruct* procstruct, LOG_PARAMS);

int _PROC_MUL (procstruct* procstruct, LOG_PARAMS);

int _PROC_DIV (procstruct* procstruct, LOG_PARAMS);

int _PROC_OUT (procstruct* procstruct, LOG_PARAMS);

int _PROC_IN  (procstruct* procstruct, LOG_PARAMS);

int _PROC_INIT(procstruct* procstruct, LOG_PARAMS);

int _PROC_HLT (procstruct* procstruct, LOG_PARAMS);

int _PROC_DUMP(procstruct* procstruct, LOG_PARAMS);

int _PROC_RET(procstruct* procstruct, LOG_PARAMS);

//===================================================================

#define DEF_JMP_(num, name, code, hash)                               \
                                                                      \
int _PROC_##name(procstruct* procstruct, LOG_PARAMS); 

#include "../text_files/jumps.txt"

#undef DEF_JMP_

// int _PROC_JMP(procstruct* procstruct, LOG_PARAMS);

// int _PROC_JA(procstruct* procstruct, LOG_PARAMS);

// int _PROC_JAE(procstruct* procstruct, LOG_PARAMS);

// int _PROC_JB(procstruct* procstruct, LOG_PARAMS);

// int _PROC_JBE(procstruct* procstruct, LOG_PARAMS);

// int _PROC_JE(procstruct* procstruct, LOG_PARAMS);

// int _PROC_JNE(procstruct* procstruct, LOG_PARAMS);

// int _PROC_JF(procstruct* procstruct, LOG_PARAMS);

//===================================================================

int _open_proc_output(const char* filename, LOG_PARAMS);

int _close_proc_output(LOG_PARAMS);

int _open_proc_input(const char* filename, LOG_PARAMS);

int _close_proc_input(LOG_PARAMS);
