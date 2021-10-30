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

#define get_argument_from_stack(value) {                              \
                                                                      \
    int err = 0;                                                      \
                                                                      \
    value = stack_pop(&procstruct->Stack, &err);                      \
    if (err == -1)                                                    \
       return -1;                                                     \
}                               

//===================================================================

#define push_argument_to_stack(value) {                               \
                                                                      \
    int err = stack_push(&procstruct->Stack, value);                  \
    if (err == -1)                                                    \
       return -1;                                                     \
}

//===================================================================

#define check_divider_is_zero(value) {                                \
                                                                      \
    if (fabs(value) < PRECISION)                                      \
       set_and_process_err(PROC_DIV_BY_ZERO);                         \
}

//===================================================================

#define is_equal(first, second) (fabs(first - second) < PRECISION)    

//===================================================================

#define procstruct_ptr_check(ptr) {                                   \
                                                                      \
    if (ptr == NULL)                                                  \
        set_and_process_err(INV_PROCSTRUCT_PTR)                       \
}

//===================================================================

#define print_value(value) {                                          \
                                                                      \
    extern FILE* proc_output;                                         \
                                                                      \
    int fprintf_ret = fprintf(proc_output, ELEM_SPEC_2 "\n", value);  \
                                                                      \
    if (fprintf_ret < 0)                                              \
       set_and_process_err(FILE_OUTPUT_ERROR);                        \
}

//===================================================================

#define scan_value(value) {                                           \
                                                                      \
    extern FILE* proc_input;                                          \
                                                                      \
    int scanf_ret = fscanf(proc_input, ELEM_SPEC, &value);            \
                                                                      \
    if (scanf_ret != 1)                                               \
       set_and_process_err(FILE_INPUT_ERR);                           \
}

//===================================================================

#define get_register_number(reg_number) {                             \
                                                                      \
    reg_number = *procstruct->ip - 1;                                 \
                                                                      \
    procstruct->ip++;                                                 \
    procstruct_ip_check(procstruct);                                  \
                                                                      \
    if (reg_number < 0 || reg_number > REGISTER_SIZE - 1)             \
       set_and_process_err(INV_REGISTER_NUMBER);                      \
}

//===================================================================

#define get_elem_t_argument(value) {                                  \
                                                                      \
    value = *(elem_t*)procstruct->ip;                                 \
                                                                      \
    procstruct->ip += sizeof(elem_t);                                 \
    procstruct_ip_check(procstruct);                                  \
}

//===================================================================

#define ram_number_check(ram_number) {                                \
                                                                      \
   if (ram_number < 0 || ram_number > RAM_SIZE)                       \
       set_and_process_err(INV_RAM_NUMBER);                           \
}

//===================================================================

#define get_address(value_ptr) {                                      \
                                                                      \
    char reg_number = 0;                                              \
    elem_t value = 0;                                                 \
                                                                      \
    switch(oper_code & ~OPER_CODE_MASK) {                             \
                                                                      \
       case (REGISTER_MASK | RAM_MASK): {                             \
                                                                      \
           get_register_number(reg_number)                            \
                                                                      \
           int ram_number = (int)(procstruct->regist[reg_number]);    \
                                                                      \
           ram_number_check(ram_number);                              \
                                                                      \
           value_ptr = (elem_t*)&procstruct->ram[ram_number];         \
                                                                      \
           break;                                                     \
       }                                                              \
                                                                      \
       case REGISTER_MASK: {                                          \
                                                                      \
           get_register_number(reg_number)                            \
                                                                      \
           value_ptr = &procstruct->regist[reg_number];               \
                                                                      \
           break;                                                     \
       }                                                              \
                                                                      \
       case (IMM_MASK | RAM_MASK): {                                  \
                                                                      \
           get_elem_t_argument(value);                                \
                                                                      \
           int ram_number = (int)value;                               \
                                                                      \
           ram_number_check(ram_number);                              \
                                                                      \
           value_ptr = (elem_t*)&procstruct->ram[ram_number];         \
                                                                      \
           break;                                                     \
       }                                                              \
                                                                      \
       case IMM_MASK: {                                               \
                                                                      \
           get_elem_t_argument(value);                                \
                                                                      \
           value_ptr = &value;                                        \
                                                                      \
           break;                                                     \
       }                                                              \
                                                                      \
       case IMM_MASK | REGISTER_MASK | RAM_MASK: {                    \
                                                                      \
           get_register_number(reg_number);                           \
           get_elem_t_argument(value);                                \
                                                                      \
           int ram_number = (int)(value                               \
                                + procstruct->regist[reg_number]);    \
                                                                      \
           ram_number_check(ram_number);                              \
                                                                      \
           value_ptr = (elem_t*)&procstruct->ram[ram_number];         \
                                                                      \
           break;                                                     \
       }                                                              \
                                                                      \
       default:                                                       \
           set_and_process_err(PROC_INV_OPER_CODE);                   \
                                                                      \
    }                                                                 \
                                                                      \
    } while(0);

//===================================================================

#define get_destination_ip(dest_ip) {                                 \
                                                                      \
       procstruct->ip++;                                              \
       procstruct_ip_check(procstruct);                               \
       procstruct_ptr_check(procstruct);                              \
                                                                      \
       dest_ip = *(int*)procstruct->ip;                               \
    }

//===================================================================

#define move_ip_to_dest(dest_ip) {                                    \
                                                                      \
    procstruct->ip = procstruct->code_array + dest_ip;                \
    procstruct_ip_check(procstruct);                                  \
}                    

//===================================================================

#define procstruct_ip_check(procstruct) {                             \
                                                                      \
    if (procstruct->ip - procstruct->code_array                       \
        > procstruct->code_file_size)                                 \
                                                                      \
    set_and_process_err(INV_INSTR_PTR)                                \
}

//===================================================================

#define init_procstruct(procstruct, fp) \
       _init_procstruct(procstruct, fp, LOG_ARGS)

#define dtor_procstruct(procstruct) \
       _dtor_procstruct(procstruct, LOG_ARGS)

#define processor_dump(procstruct) \
       _processor_dump(procstruct, LOG_ARGS)
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

// #define PROC_ADD(procstruct, oper_code) \
//        _PROC_ADD(procstruct, LOG_ARGS)

// #define PROC_SUB(procstruct, oper_code) \
//        _PROC_SUB(procstruct, LOG_ARGS)

// #define PROC_MUL(procstruct, oper_code) \
//        _PROC_MUL(procstruct, LOG_ARGS)

// #define PROC_DIV(procstruct, oper_code) \
//        _PROC_DIV(procstruct, LOG_ARGS)

// #define PROC_PUSH(procstruct, oper_code) \
//        _PROC_PUSH(procstruct, oper_code,  LOG_ARGS)

// #define PROC_OUT(procstruct, oper_code) \
//        _PROC_OUT(procstruct, LOG_ARGS)

// #define PROC_POP(procstruct, oper_code) \
//        _PROC_POP(procstruct, oper_code, LOG_ARGS)

// #define PROC_IN(procstruct, oper_code) \
//        _PROC_IN(procstruct, LOG_ARGS)

// #define PROC_HLT(procstruct, oper_code) \
//        _PROC_HLT(procstruct, LOG_ARGS)

// #define PROC_DUMP(procstruct, oper_code) \
//        _PROC_DUMP(procstruct, LOG_ARGS)

// #define PROC_RET(procstruct, oper_code) \
//        _PROC_RET(procstruct, LOG_ARGS)

//===================================================================

// #define PROC_JMP(procstruct) \
//        _PROC_JMP(procstruct, LOG_ARGS)

// #define PROC_JA(procstruct) \
//        _PROC_JA(procstruct, LOG_ARGS)

// #define PROC_JAE(procstruct) \
//        _PROC_JAE(procstruct, LOG_ARGS)

// #define PROC_JB(procstruct) \
//        _PROC_JB(procstruct, LOG_ARGS)

// #define PROC_JBE(procstruct) \
//        _PROC_JBE(procstruct, LOG_ARGS)

// #define PROC_JE(procstruct) \
//        _PROC_JE(procstruct, LOG_ARGS)

// #define PROC_JNE(procstruct) \
//        _PROC_JNE(procstruct, LOG_ARGS)

// #define PROC_JF(procstruct) \
//        _PROC_JF(procstruct, LOG_ARGS)

// #define PROC_CALL(procstruct) \
//        _PROC_CALL(procstruct, LOG_ARGS)

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

// #define proc_dump(procstruct) \
//        _PROC_DUMP(procstruct, LOG_ARGS)

#define proc_final_check(procstruct) \
       _proc_final_check(procstruct, LOG_ARGS)

//===================================================================

int _proc_final_check(procstruct* procstruct, LOG_PARAMS);

int _processor_dump(procstruct* procstruct, LOG_PARAMS);

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

// int _PROC_PUSH(procstruct* procstruct, unsigned char oper_code, LOG_PARAMS);

// int _PROC_POP (procstruct* procstruct, unsigned char oper_code, LOG_PARAMS);

// int _PROC_ADD (procstruct* procstruct, LOG_PARAMS);

// int _PROC_SUB (procstruct* procstruct, LOG_PARAMS);

// int _PROC_MUL (procstruct* procstruct, LOG_PARAMS);

// int _PROC_DIV (procstruct* procstruct, LOG_PARAMS);

// int _PROC_OUT (procstruct* procstruct, LOG_PARAMS);

// int _PROC_IN  (procstruct* procstruct, LOG_PARAMS);

// int _PROC_HLT (procstruct* procstruct, LOG_PARAMS);

// int _PROC_DUMP(procstruct* procstruct, LOG_PARAMS);

// int _PROC_RET(procstruct* procstruct, LOG_PARAMS);

//===================================================================

// #define DEF_JMP_(num, name, code, hash)                               \
//                                                                       \
// int _PROC_##name(procstruct* procstruct, LOG_PARAMS); 

// #include "../text_files/jumps.txt"

// #undef DEF_JMP_

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

// FILE* open_log_file(const char* filename);

// int close_log_file(FILE* logs_file);
