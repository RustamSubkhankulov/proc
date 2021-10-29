#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "processor.h"
#include "../stack/stackconfig.h"

//===================================================================

int _open_proc_input(const char* filename, LOG_PARAMS) {

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

int _close_proc_input(LOG_PARAMS) {

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

int _open_proc_output(const char* filename, LOG_PARAMS) {

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

int _close_proc_output(LOG_PARAMS) {

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

int _header_check(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    memcpy(&procstruct->header, procstruct->code_array, sizeof(struct Header));

    if (procstruct->header.signature != SIGN)
        set_and_process_err(HDR_INV_SIGN);

    if (procstruct->header.version != VERSION)
        set_and_process_err(HDR_INV_VERSION);

    if (procstruct->header.file_size != procstruct->code_file_size)
        set_and_process_err(HDR_INV_FILE_SIZE);

    return 0;
}

//===================================================================

int _init_procstruct(procstruct* procstruct, FILE* fp, LOG_PARAMS) {

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


int _dtor_procstruct(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);
    procstruct_ip_check(procstruct);

    if (procstruct->code_array == NULL)
        set_and_process_err(INV_CODE_ARRAY_PTR);

    free((void*)procstruct->code_array);

    return 0;
}
//===================================================================

const unsigned char* _read_from_file(FILE* fp, long* size, LOG_PARAMS) {

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

int _proc_perform(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    while (procstruct->ip - procstruct->code_array 
                         <= procstruct->code_file_size) {

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

#define DEF_JMP_(num, name, code, hash)                             \
                                                                    \
    case code: {                                                    \
                                                                    \
        int ret_val = PROC_##name(procstruct);                      \
        if (ret_val == -1)                                          \
            return -1;                                              \
                                                                    \
        break;                                                      \
    }                                                                

//===================================================================

int _proc_execute_command(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);
    procstruct_ip_check(procstruct);

    unsigned char oper_code = *(procstruct->ip);

    printf("\n\n %02x \n\n", oper_code);

    switch (oper_code & OPER_CODE_MASK) {

        #include "../text_files/commands.txt"
        #include "../text_files/jumps.txt"

        #undef DEF_CMD_
        #undef DEF_JMP_

        default: 
            set_and_process_err(PROC_INV_OPER_CODE);
    }

    return 0;
}

//===================================================================

int _PROC_RET(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    int err = 0;

    int ret_ip = stack_pop(&procstruct->Stack, &err);
    if (err == -1)
        return -1;

    procstruct->ip = procstruct->code_array + ret_ip;
    procstruct_ip_check(procstruct);

    procstruct->command_ct;

    return 0;
}

//===================================================================

int _PROC_CALL(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    int dest_ip = 0;

    get_destination_ip;

    procstruct->ip += sizeof(int);
    procstruct_ip_check(procstruct);

    int return_ip = procstruct->ip - procstruct->code_array;

    int err = stack_push(&procstruct->Stack, return_ip);
    if (err == -1)
        return -1;

    procstruct->ip = procstruct->code_array + dest_ip;
    procstruct_ip_check(procstruct);

    procstruct->command_ct++;

    return 0;
}

//===================================================================

int _PROC_JMP(procstruct* procstruct, LOG_PARAMS) {   
                                                                    
    proc_log_report(); 

    int dest_ip = 0;                                             
                                                                    
    get_destination_ip;                        
                                                                           
    procstruct->ip = procstruct->code_array + dest_ip;         
    procstruct_ip_check(procstruct);                            
                                                     
    procstruct->command_ct++;                                      
                                                                    
    return 0;                                                       
}

//===================================================================

int _PROC_JA(procstruct* procstruct, LOG_PARAMS) {   
                                                                    
    proc_log_report();                                              
                                                                    
    int dest_ip = 0;

    get_destination_ip;                              
                                                                    
    int err = 0;                                                    
    elem_t first_val = 0;
    elem_t second_val = 0;    

    get_two_args_from_stack;                                                                     
                                                                    
    if (first_val > second_val) {           
                                                                    
        procstruct->ip = procstruct->code_array + dest_ip;          
        procstruct_ip_check(procstruct);                            
    }                                                               
    else {                                                          
                                                                    
        procstruct->ip += sizeof(int);                           
        procstruct_ip_check(procstruct);                            
        }                                                           
                                                                    
    procstruct->command_ct++;                                       
                                                                    
    return 0;                                                       
}

//===================================================================

int _PROC_JAE(procstruct* procstruct, LOG_PARAMS) {   
                                                                    
    proc_log_report();                                              
                                                                    
    int dest_ip = 0;

    get_destination_ip;                              
                                                                    
    int err = 0;                                                    
    elem_t first_val = 0;
    elem_t second_val = 0;    

    get_two_args_from_stack;                         
                                                                    
    if (first_val >= second_val) {           
                                                                    
        procstruct->ip = procstruct->code_array + dest_ip;          
        procstruct_ip_check(procstruct);                            
    }                                                               
    else {                                                          
                                                                    
        procstruct->ip += sizeof(int);                           
        procstruct_ip_check(procstruct);                            
        }                                                           
                                                                    
    procstruct->command_ct++;                                       
                                                                    
    return 0;                                                       
}

//===================================================================

int _PROC_JB(procstruct* procstruct, LOG_PARAMS) {   
                                                                    
    proc_log_report();                                              
                                                                    
    int dest_ip = 0;

    get_destination_ip;                              
                                                                    
    int err = 0;                                                    
    elem_t first_val = 0;
    elem_t second_val = 0;    

    get_two_args_from_stack;                         
                                                                    
    if (first_val < second_val) {           
                                                                    
        procstruct->ip = procstruct->code_array + dest_ip;          
        procstruct_ip_check(procstruct);                            
    }                                                               
    else {                                                          
                                                                    
        procstruct->ip += sizeof(int);                           
        procstruct_ip_check(procstruct);                            
        }                                                           
                                                                    
    procstruct->command_ct++;                                       
                                                                    
    return 0;                                                       
}

//===================================================================

int _PROC_JBE(procstruct* procstruct, LOG_PARAMS) {   
                                                                    
    proc_log_report();                                              
                                                                    
    int dest_ip = 0;

    get_destination_ip;                              
                                                                    
    int err = 0;                                                    
    elem_t first_val = 0;
    elem_t second_val = 0;    

    get_two_args_from_stack;                         
                                                                    
    if (first_val <= second_val) {           
                                                                    
        procstruct->ip = procstruct->code_array + dest_ip;          
        procstruct_ip_check(procstruct);                            
    }                                                               
    else {                                                          
                                                                    
        procstruct->ip += sizeof(int);                           
        procstruct_ip_check(procstruct);                            
        }                                                           
                                                                    
    procstruct->command_ct++;                                       
                                                                    
    return 0;                                                       
}

//===================================================================

int _PROC_JE(procstruct* procstruct, LOG_PARAMS) {   
                                                                    
    proc_log_report();                                              
                                                                    
    int dest_ip = 0;

    get_destination_ip;                              
                                                                    
    int err = 0;                                                    
    elem_t first_val = 0;
    elem_t second_val = 0;    

    get_two_args_from_stack;                       
     //                                                               
    if (fabs(first_val - second_val) < PRECISION) {         
                                                                    
        procstruct->ip = procstruct->code_array + dest_ip;          
        procstruct_ip_check(procstruct);                            
    }                                                               
    else {                                                          
                                                                    
        procstruct->ip += sizeof(int);                           
        procstruct_ip_check(procstruct);                            
        }                                                           
                                                                    
    procstruct->command_ct++;                                       
                                                                    
    return 0;                                                       
}

///==================================================================

int _PROC_JNE(procstruct* procstruct, LOG_PARAMS) {   
                                                                    
    proc_log_report();                                              
                                                                    
    int dest_ip = 0;

    get_destination_ip;                              
                                                                    
    int err = 0;                                                    
    elem_t first_val = 0;
    elem_t second_val = 0;    

    get_two_args_from_stack;                        
                                                                    
    if (fabs(first_val - second_val) > 0) {           
                                                                    
        procstruct->ip = procstruct->code_array + dest_ip;          
        procstruct_ip_check(procstruct);                            
    }                                                               
    else {                                                          
                                                                    
        procstruct->ip += sizeof(int);                           
        procstruct_ip_check(procstruct);                            
        }                                                           
                                                                    
    procstruct->command_ct++;                                       
                                                                    
    return 0;                                                       
}

//===================================================================

int _PROC_INIT(procstruct* procstruct, LOG_PARAMS) {

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

int _PROC_HLT(procstruct* procstruct, LOG_PARAMS) {

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

int _PROC_ADD(procstruct* procstruct, LOG_PARAMS) {

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

int _PROC_SUB(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    int err = 0;

    elem_t first_value  = stack_pop(&procstruct->Stack, &err);
    if (err == -1) 
        return -1;

    elem_t second_value = stack_pop(&procstruct->Stack, &err); 
    if (err == -1) 
        return -1;

    err = stack_push(&procstruct->Stack, second_value - first_value);

    if (err == -1) 
        return -1;

    else {
        procstruct->command_ct++;
        procstruct->ip++;

        return 0;
    }
}
//===================================================================

int _PROC_DIV(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    int err = 0;

    elem_t divider  = stack_pop(&procstruct->Stack, &err);
    elem_t dividend = stack_pop(&procstruct->Stack, &err);

    if (err == -1) 
        return -1;
    
    if (fabs(divider) < PRECISION)
        set_and_process_err(PROC_DIV_BY_ZERO);

    err = stack_push(&procstruct->Stack, (elem_t)(dividend / divider));
 
    if (err == -1) 
        return -1;

    else {
        procstruct->command_ct++;
        procstruct->ip++;

        return 0;
    }
}
//===================================================================

int _PROC_MUL(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    int err = 0;

    elem_t value = stack_pop(&procstruct->Stack, &err)
                 * stack_pop(&procstruct->Stack, &err);

    if (err == -1) 
        return -1;

    err = stack_push(&procstruct->Stack, value);

    if (err == -1) 
        return -1;

    else {
        procstruct->command_ct++;
        procstruct->ip++;

        return 0;
    }
}
//===================================================================

int _PROC_OUT(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    extern FILE* proc_output;
    int err = 0;

    elem_t out_value = stack_pop(&procstruct->Stack, &err);

    if (err == -1) 
        return -1;

    int fprintf_ret = fprintf(proc_output, ELEM_SPEC_2 "\n", out_value);

    if (fprintf_ret < 0)
        set_and_process_err(FILE_OUTPUT_ERROR)

    else {
        procstruct->command_ct++;
        procstruct->ip++;

        return 0;
    }
}
//===================================================================

int _PROC_IN(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    extern FILE* proc_input;

    elem_t in_value = 0;

    int fscanf_ret = fscanf(proc_input, ELEM_SPEC, &in_value);

    if (fscanf_ret != 1)
        set_and_process_err(FILE_INPUT_ERR);

    int err = stack_push(&procstruct->Stack, in_value);

    if (err == -1) 
        return -1;

    else {
        procstruct->command_ct++;
        procstruct->ip++;

        return 0;
    }
}
//===================================================================

int _PROC_PUSH(procstruct* procstruct, unsigned char oper_code, LOG_PARAMS) {

    proc_log_report();

    procstruct->ip++;
    procstruct_ip_check(procstruct);

    int err = 0;
    oper_code = oper_code & ~OPER_CODE_MASK;

    switch (oper_code & ~RAM_MASK) {

    case REGISTER_MASK: {

        char reg_number = *procstruct->ip - 1;
        
        procstruct->ip++;
        procstruct_ip_check(procstruct);

        if (reg_number < 0 || reg_number > REGISTER_SIZE - 1)
            set_and_process_err(INV_REGISTER_NUMBER);

        if ((oper_code & ~REGISTER_MASK) == RAM_MASK) {
            
            if (procstruct->regist[reg_number] > RAM_SIZE)
                set_and_process_err(RAM_INV_ADDRESS);
//

//
            err = stack_push(&procstruct->Stack,
                             procstruct->ram
                             [(int)procstruct->regist[reg_number]]);
        }
        else 
            err = stack_push(&procstruct->Stack,
                              procstruct->regist[reg_number]);
        break;
    }

    case IMM_MASK: {

        elem_t value = *(elem_t*)procstruct->ip;
        
        procstruct->ip += sizeof(elem_t);
        procstruct_ip_check(procstruct);

        if ((oper_code & ~IMM_MASK) == RAM_MASK) {

            if (value > RAM_SIZE)
                set_and_process_err(RAM_INV_ADDRESS);
////
            err = stack_push(&procstruct->Stack,
                                procstruct->ram[(int)value]);
        }
        else
            err = stack_push(&procstruct->Stack, value);

        break;
    }

    case IMM_MASK | REGISTER_MASK: {
        
        if (oper_code & ~(IMM_MASK | REGISTER_MASK) != RAM_MASK)
            set_and_process_err(PROC_INVALID_CMND);

        int ram_number = 0;
        get_ram_number;

        int err = stack_push(&procstruct->Stack, 
                             procstruct->ram[ram_number]);
        if (err == -1)
            set_and_process_err(STACK_PROC_ERROR);

        break;
    }

    default: 
        set_and_process_err(PROC_INV_OPER_CODE);
    }

    if (err == -1) 
        return -1;

    procstruct->command_ct++;
    return 0;
}

//===================================================================

int _PROC_POP(procstruct* procstruct, unsigned char oper_code, LOG_PARAMS) {

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
        
        if ((oper_code & ~REGISTER_MASK) == RAM_MASK) {

            if (procstruct->regist[reg_number] > RAM_SIZE)
                set_and_process_err(RAM_INV_ADDRESS);
//
            procstruct->ram[(int)procstruct->regist[reg_number]] = 
                stack_pop(&procstruct->Stack, &err);
        }
        else 
            procstruct->regist[reg_number] =
                stack_pop(&procstruct->Stack, &err);

        break;
    }

    case IMM_MASK: {

        elem_t value = *(elem_t*)procstruct->ip;
        procstruct->ip += sizeof(elem_t);

        if ((oper_code & ~IMM_MASK) == RAM_MASK) {

            if (value > RAM_SIZE)
                set_and_process_err(RAM_INV_ADDRESS);
//
            procstruct->ram[(int)value] = 
                stack_pop(&procstruct->Stack, &err);
        }
        else
            set_and_process_err(PROC_INV_OPER_CODE);

        break;
    }

    case IMM_MASK | REGISTER_MASK: {

        if (oper_code & ~(IMM_MASK | REGISTER_MASK) != RAM_MASK)
            set_and_process_err(PROC_INVALID_CMND);

        int ram_number = 0;
        get_ram_number;

        int err = 0;
        elem_t value = stack_pop(&procstruct->Stack, &err);

        if (err == -1)
            return -1;

        procstruct->ram[ram_number] = value;

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

int _proc_final_check(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    if (procstruct->ip - procstruct->code_array
        > procstruct->code_file_size
     || procstruct->ip - procstruct->code_array
        < 0)

        set_and_process_err(INV_INSTR_PTR);

    return 0;
}

//===================================================================

int _PROC_DUMP(procstruct* procstruct, LOG_PARAMS) {

    proc_log_report();

    procstruct_ptr_check(procstruct);

    extern FILE* log_output;

    fprintf(log_output, "\n" "Commands executed %d" "\n", procstruct->command_ct);

    fprintf(log_output, "\n" "Size of code array: %lu", procstruct->code_file_size);

    fprintf(log_output, "\n" "Code array address <%p>" "\n", procstruct->code_array);


    if (procstruct->code_file_size != procstruct->header.file_size)

        fprintf(log_output, "\n" "ERROR: incorrect code file size:");

        fprintf(log_output, "\n" "File size written in header during compiling: %ld",
                                                        procstruct->header.file_size);

        fprintf(log_output, "\n" "Size of file copied during processing: %ld",
                                                   procstruct->code_file_size);

    if (procstruct->code_array == NULL) {

        fprintf(log_output, "\n" "ERROR: invalid pointer to the code array");
        return -1;
    }

    fprintf(log_output, "\n" "Instruction pointer: %ld" "\n", 
                        procstruct->ip - procstruct->code_array);

    if (procstruct->ip - procstruct->code_array > procstruct->code_file_size) {

        fprintf(log_output, "\n" "ERROR: instruction pointer points out of code array");
    }

    fprintf(log_output, "\n" "Code array: " "\n\n");

    int ip_value = procstruct->ip - procstruct->code_array;
    int val = 0;

    for (int ct = 0; ct < procstruct->code_file_size;) {

        fprintf(log_output, "%02x ", procstruct->code_array[ct++]);

        if ((ct / 16) * 16 + 16 > (ip_value / 16) * 16 + 16 && val == 0) {

            fprintf(log_output, "\n" "%*c\n", (ip_value % 16) * 3 + 1, '^');
            val++;
        }

        else if ((ct % 16) == 0)
            fprintf(log_output, "\n\n");
    }

    fprintf(log_output, "\n\n");

    if (stack_validator(&procstruct->Stack) == -1) {

        fprintf(log_output, "\n" "Error in stack work" "\n");
        stack_dump(&procstruct->Stack);
    }
    else {

        fprintf(log_output, "\n" "No errors in stack working" "\n");
        stack_out(&procstruct->Stack);
    }

    fprintf(log_output, "\n\n" "Registers: " "\n\n");

    for (int ct = 0; ct < REGISTER_SIZE; ct++) {

        fprintf(log_output, "[%c] %g ", ct + ASCII_OFFSET + 1,
                                      procstruct->regist[ct]);
    
        if ( (ct + 1) % 8 == 0)
        fprintf(log_output, "\n\n");
    }

    fprintf(log_output, "\n\n");

    procstruct->command_ct++;

    procstruct->ip++;
    procstruct_ip_check(procstruct);

    return 0;
}
