#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <stdlib.h>

#include "processor.h"

//===================================================================

int _open_proc_input(const char* filename, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    if (filename == NULL) 
        set_and_process_err(INV_FILE_NAME);

    extern FILE* proc_input;

    FILE* fp = fopen(filename, "rb");

    if (fp == NULL) 
        set_and_process_err(FOPEN_ERROR);

    proc_input = fp;

    return 0;
}

//==================================================================

int _close_proc_input(FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    extern FILE* proc_input;

    if (proc_input != stdin) {

        int fclose_ret = fclose(proc_input);

        if (fclose_ret == EOF)
            set_and_process_err(FCLOSE_ERROR);
    }

    return 0;
}

//===================================================================

int _open_proc_output(const char* filename, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    if (filename == NULL)
        set_and_process_err(INV_FILE_NAME);

    extern FILE* proc_output;

    FILE* fp = fopen(filename, "wb");

    if (fp == NULL)
        set_and_process_err(INV_FILE_PTR);

    proc_output = fp;

    return 0;
}

//===================================================================

int _close_proc_output(FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    extern FILE* proc_output;

    if (proc_output != stdout) {

        int fclose_ret = fclose(proc_output);

        if (fclose_ret == EOF)
            set_and_process_err(FCLOSE_ERROR);
    }

    return 0;
}

//===================================================================

int _header_check(procstruct* procstruct, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    memcpy(&procstruct->header, procstruct->code_array, sizeof(struct Header));

    if (procstruct->header.signature != SIGN)
        set_and_process_err(HDR_INV_SIGN);

    if (procstruct->header.version != VERSION)
        set_and_process_err(HDR_INV_VERSION);

    if (procstruct->header.file_size != procstruct->code_file_size)
        set_and_process_err(HDR_INV_FILE_SIZE);

    if (procstruct->header.elem_t_args_number
      + procstruct->header.register_args_number
      > procstruct->header.commands_number)

        set_and_process_err(HDR_INV_CMD_NUMBERS);

    return 0;
}

//===================================================================

int _init_procstruct(procstruct* procstruct, FILE* fp, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    file_ptr_check(fp);
    procstruct_ptr_check(procstruct);

    long code_file_size = 0;

    const unsigned char* code_array = read_from_file(fp, &code_file_size);

    char_ptr_check(code_array);

    procstruct->code_array = code_array;
    procstruct->code_file_size = code_file_size;

    int header_check_ret = header_check(procstruct);
    if (header_check_ret == -1)
        return -1;

    procstruct->ip = procstruct->code_array + sizeof(struct Header);

    return 0;
}

//===================================================================


int _dtor_procstruct(procstruct* procstruct, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);
    procstruct_ip_check(procstruct);

    if (procstruct->code_array == NULL)
        set_and_process_err(INV_CODE_ARRAY_PTR);

    free((void*)procstruct->code_array);

    return 0;
}
//===================================================================

const unsigned char* _read_from_file(FILE* fp, long* size, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    if (fp == NULL) {

        set_global_error_code(INV_FILE_PTR);
        global_error_process();

        return NULL;
    }

    long code_file_size = file_size(fp);
    if ( code_file_size < 0)
        return NULL;

    if (size) *size = code_file_size;

    const unsigned char* code_array = 
    (const unsigned char*)copy_data_to_buf(code_file_size, fp);

    if (code_array == NULL)
        return NULL;

    return code_array;
}

//===================================================================

int _proc_perform(procstruct* procstruct, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    for (int cmnd_counter = 0;
             cmnd_counter < procstruct->header.commands_number;
             cmnd_counter ++) {

        int ret_val = proc_execute_command(procstruct);

        if (ret_val == -1) {
            
            proc_dump(procstruct);
            return -1;
        }

        if (ret_val == 1)
            break;
    }

    int final_check_value = proc_final_check(procstruct);
    if (final_check_value == -1)
        return -1;

    return 0;
}

//===================================================================

#define DEF_CMD_(num, name, code, hash)                             \
                                                                    \
    case code: {                                                    \
                                                                    \
        int ret_val = PROC_##name(procstruct, oper_code);           \
        if (ret_val == -1)                                          \
            return -1;                                              \
                                                                    \
        if (code == 0)                                              \
            return 1;                                               \
                                                                    \
        break;                                                      \
        }


//===================================================================

int _proc_execute_command(procstruct* procstruct, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);
    procstruct_ip_check(procstruct);

    unsigned char oper_code = *(procstruct->ip);

    printf("\n\n %02x \n\n", oper_code);

    switch (oper_code & OPER_CODE_MASK) {

        #include "../text_files/commands.txt"

        #undef DEF_CMD_

        default: 
            set_and_process_err(PROC_INV_OPER_CODE);
    }

    return 0;
}

//===================================================================

int _PROC_INIT(procstruct* procstruct, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct); 

    int ret_val = stack_ctor(&procstruct->Stack);
    
    if (ret_val == -1)
        return -1;

    else {
        procstruct->command_ct++;
        procstruct->ip++;

        return 0;
    }
}
//===================================================================

int _PROC_HLT(procstruct* procstruct, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct); 

    int stack_dtor_ret =  stack_dtor(&procstruct->Stack);

    if (stack_dtor_ret == -1)
        return -1;

    else {
        procstruct->command_ct++;
        procstruct->ip++;

        return 1;
    }
}

//===================================================================

int _PROC_ADD(procstruct* procstruct, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    int err = 0;

    elem_t value = stack_pop(&procstruct->Stack, &err)
                 + stack_pop(&procstruct->Stack, &err);

    if (err == -1) return -1;

    err = stack_push(&procstruct->Stack, value);

    if (err == -1) return -1;

    else {
        procstruct->command_ct++;
        procstruct->ip++;

        return 0;
    }
}

//===================================================================

int _PROC_SUB(procstruct* procstruct, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    int err = 0;

    elem_t first_value  = stack_pop(&procstruct->Stack, &err);
    if (err == -1) return -1;

    elem_t second_value = stack_pop(&procstruct->Stack, &err); 
    if (err == -1) return -1;

    err = stack_push(&procstruct->Stack, second_value - first_value);

    if (err == -1) return -1;

    else {
        procstruct->command_ct++;
        procstruct->ip++;

        return 0;
    }
}
//===================================================================

int _PROC_DIV(procstruct* procstruct, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    int err = 0;

    elem_t divider  = stack_pop(&procstruct->Stack, &err);
    elem_t dividend = stack_pop(&procstruct->Stack, &err);

    if (err == -1) return -1;
    
    if (divider == 0)
        set_and_process_err(PROC_DIV_BY_ZERO);

    err = stack_push(&procstruct->Stack, (elem_t)(dividend / divider));
 
    if (err == -1) return -1;

    else{
        procstruct->command_ct++;
        procstruct->ip++;

        return 0;
    }
}
//===================================================================

int _PROC_MUL(procstruct* procstruct, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    int err = 0;

    elem_t value = stack_pop(&procstruct->Stack, &err)
                 * stack_pop(&procstruct->Stack, &err);

    if (err == -1) return -1;

    err = stack_push(&procstruct->Stack, value);

    if (err == -1) return -1;

    else {
        procstruct->command_ct++;
        procstruct->ip++;

        return 0;
    }
}
//===================================================================

int _PROC_OUT(procstruct* procstruct, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    extern FILE* proc_output;
    int err = 0;

    elem_t out_value = stack_pop(&procstruct->Stack, &err);

    if (err == -1) return -1;

    int fprintf_ret = fprintf(proc_output, ELEM_SPEC "\n", out_value);

    if (fprintf_ret < 0)
        set_and_process_err(FILE_OUTPUT_ERROR)

    else {
        procstruct->command_ct++;
        procstruct->ip++;

        return 0;
    }
}
//===================================================================

int _PROC_IN(procstruct* procstruct, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    extern FILE* proc_input;

    elem_t in_value = 0;

    int fscanf_ret = fscanf(proc_input, ELEM_SPEC, &in_value);

    if (fscanf_ret != 1)
        set_and_process_err(FILE_INPUT_ERR);

    int err = stack_push(&procstruct->Stack, in_value);

    if (err == -1) return -1;

    else {
        procstruct->command_ct++;
        procstruct->ip++;

        return 0;
    }
}
//===================================================================

int _PROC_PUSH(procstruct* procstruct, unsigned char oper_code, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    procstruct->ip++;
    procstruct_ip_check(procstruct);

    int err = 0;
    oper_code = oper_code & ~OPER_CODE_MASK;

    switch (oper_code & ~RAM_MASK) {

    case REGISTER_MASK: {

        char reg_number = *procstruct->ip - 1;
        procstruct->ip++;

        if (reg_number < 0 || reg_number > REGISTER_SIZE - 1)
            set_and_process_err(INV_REGISTER_NUMBER);

        if (oper_code & ~REGISTER_MASK == RAM_MASK) {
            
            if (procstruct->regist[reg_number] > RAM_SIZE)
                set_and_process_err(RAM_INV_ADDRESS);

            err = stack_push(&procstruct->Stack,
                              procstruct->ram
                             [procstruct->regist[reg_number]]);

            procstruct->ram_using_commands_number++;
        }
        else 
            err = stack_push(&procstruct->Stack,
                              procstruct->regist[reg_number]);

        procstruct->register_args_ct++;
        break;
    }

    case IMM_MASK: {

        elem_t value = *(elem_t*)procstruct->ip;
        procstruct->ip += sizeof(elem_t);

        if (oper_code & ~IMM_MASK == RAM_MASK) {

            if (value > RAM_SIZE)
                set_and_process_err(RAM_INV_ADDRESS);

            err = stack_push(&procstruct->Stack,
                                procstruct->ram[value]);

            procstruct->ram_using_commands_number++;
        }
        else
            err = stack_push(&procstruct->Stack, value);

        procstruct->elem_t_args_ct++;
        break;
    }

    default: 
        set_and_process_err(PROC_INV_OPER_CODE);
    }

    if (err == -1) return -1;

    procstruct->command_ct++;
    return 0;
}

//===================================================================

int _PROC_POP(procstruct* procstruct, unsigned char oper_code, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    procstruct->ip++;
    procstruct_ip_check(procstruct);

    oper_code = oper_code & ~OPER_CODE_MASK;
    int err = 0;

    switch(oper_code & ~RAM_MASK) {

    case REGISTER_MASK: {

        char reg_number = *procstruct->ip - 1;
        procstruct->ip++;

        if (reg_number < 0 || reg_number > REGISTER_SIZE - 1)
            set_and_process_err(INV_REGISTER_NUMBER);
        
        if (oper_code & ~REGISTER_MASK == RAM_MASK) {

            if (procstruct->regist[reg_number] > RAM_SIZE)
                set_and_process_err(RAM_INV_ADDRESS);

            procstruct->ram[procstruct->regist[reg_number]] = 
                stack_pop(&procstruct->Stack, &err);

            procstruct->ram_using_commands_number++;
        }
        else 
            procstruct->regist[reg_number] =
                stack_pop(&procstruct->Stack, &err);

        procstruct->register_args_ct++;
        break;
    }

    case IMM_MASK: {

        elem_t value = *(elem_t*)procstruct->ip;
        procstruct->ip += sizeof(elem_t);

        if (oper_code & ~IMM_MASK == RAM_MASK) {

            if (value > RAM_SIZE)
                set_and_process_err(RAM_INV_ADDRESS);

            procstruct->ram[value] = 
                stack_pop(&procstruct->Stack, &err);

            procstruct->ram_using_commands_number++;
        }
        else
            set_and_process_err(PROC_INV_OPER_CODE);

        procstruct->elem_t_args_ct++;
        break;
    }

    default:
        set_and_process_err(PROC_INV_OPER_CODE);
    }

    if (err == -1) return -1;

    procstruct->command_ct++;
    return 0;
}

//===================================================================

int _proc_final_check(procstruct* procstruct, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    if (procstruct->ip - procstruct->code_array
        > procstruct->code_file_size)

        set_and_process_err(INV_INSTR_PTR);

    if (procstruct->command_ct !=
        procstruct->header.commands_number)

        set_and_process_err(INV_COMMANDS_NUMBER);

    if (procstruct->elem_t_args_ct !=
        procstruct->header.elem_t_args_number)

        set_and_process_err(INV_ARGUMENTS_NUMBER);

    if (procstruct->register_args_ct !=
        procstruct->header.register_args_number)

        set_and_process_err(INV_REGISTER_ARGS_NUMBER);

    if (procstruct->ram_using_commands_number !=
        procstruct->header.ram_using_commands_number)

        set_and_process_err(RAM_COMMANDS_INV_NUMBER);

    return 0;
}

//===================================================================

int _PROC_DUMP(procstruct* procstruct, FUNC_FILE_LINE_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    extern FILE* error_file;

    fprintf(error_file, "\n" "Critical error during processing." "\n");

    fprintf(error_file, "\n" "Size of code array: %lu", procstruct->code_file_size);

    fprintf(error_file, "\n" "Code array address <%p>" "\n", procstruct->code_array);

    fprintf(error_file, "\n" "Commands executed: %d of %d",
                                                procstruct->command_ct,
                                                procstruct->header.commands_number);
    fprintf(error_file, "\n" "Arguments used during executing: %d of %d",
                                                procstruct->elem_t_args_ct,
                                                procstruct->header.elem_t_args_number);
    fprintf(error_file, "\n" "Register arguments used during executing: %d of %d",
                                            procstruct->elem_t_args_ct,
                                            procstruct->header.elem_t_args_number);

    if (procstruct->command_ct > procstruct->header.commands_number) 

        fprintf(error_file, "\n" "ERROR: incorrect number of executed commands");
    

    if (procstruct->elem_t_args_ct > procstruct->header.elem_t_args_number) 

        fprintf(error_file, "\n" "ERROR: incorrect number of arguments used");

    if (procstruct->code_file_size != procstruct->header.file_size)

        fprintf(error_file, "\n" "ERROR: incorrect code file size:");

        fprintf(error_file, "\n" "File size written in header during compiling: %ld",
                                                        procstruct->header.file_size);

        fprintf(error_file, "\n" "Size of file copied during processing: %ld",
                                                   procstruct->code_file_size);

    if (procstruct->code_array == NULL) {

        fprintf(error_file, "\n" "ERROR: invalid pointer to the code array");
        return -1;
    }

    if (procstruct->ip - procstruct->code_array > procstruct->code_file_size) {

        fprintf(error_file, "\n" "ERROR: instruction pointer points out of code array");
    }

    fprintf(error_file, "\n" "Code array: " "\n\n");

    int ip_value = procstruct->ip - procstruct->code_array;
    int val = 0;

    for (int ct = 0; ct < procstruct->code_file_size;) {

        fprintf(error_file, "%02x ", procstruct->code_array[ct++]);

        if ((ct / 16) * 16 + 16 > (ip_value / 16) * 16 + 16 && val == 0) {

            fprintf(error_file, "\n" "%*c\n", (ip_value % 16) * 3 + 1, '^');
            val++;
        }

        else if ((ct % 16) == 0)
            fprintf(error_file, "\n\n");
    }

    fprintf(error_file, "\n\n");

    if (stack_validator(&procstruct->Stack) == -1) {

        fprintf(error_file, "\n" "Error in stack work" "\n");
        stack_dump(&procstruct->Stack);
    }
    else {

        fprintf(error_file, "\n" "No errors in stack working" "\n");
        stack_out(&procstruct->Stack);
    }

    return 0;
}
