#pragma once

#define STK_CANARY1_ERR_		"Left canary protection has fallen. Save yourself if you can"

#define STK_CANARY2_ERR_		"Right canary protection has fallen. Save yourself if you can"

#define STK_DATA_CANARY1_ERR_   "Left data canary protection has fallen.\n"\
								"Save yourself if you can"

#define STK_DATA_CANARY2_ERR_   "Right data canary protection has fallen.\n"\
								"Save yourself if you can"

#define STK_INVALSIZE_			"Invalid size of stack"

#define STK_NO_MEM_             "Can not find free memory"

#define STK_OVRFLW_             "Max capacity of stack was reached"

#define STK_UNDFLW_             "Stack is empty"

#define STK_NO_ELEMS_PUSHED_    "No elements have been pushed to the stack"

#define STK_INV_NEW_SIZE_       "Invalid new size in stack_set_size"

#define STK_CTOR_UNPREPARED_    "Stack is not prepared for stack constructor"

#define STK_SZ_LESS_CT_         "New size of stack in 'stack_resize' can not be less\n"\
								"than number of elements in it"

#define STK_UNKNOWN_RESZ_PARAM_ "Unknown resize parameter"

#define STK_INV_SZ_ELEM_        "Invalid size of elem_t"

#define STK_INV_CAPACITY_       "Invalid capacity of the stack"

#define STK_INV_ELEM_CT_        "Invalid number of elements in stack"

#define STK_CAPACITY_LESS_CT_   "Stack: capacity can not be less than number of elements in it"

#define STK_INV_FREE_DATA_      "Stack: free data for elements is not valid\n"\
								"every element in free data must equal 0"

#define STK_INV_DATA_PTR_       "Invalid data pointer is stuck structure"

#define STK_POISON_CHECK_       "Stack poison check failed"

#define STK_EMPTY_CHECK_		"Stack empty check failed"

#define STK_INV_ORIG_FUNC_		"Invalid stack origin function name"

#define STK_INV_ORIG_FILE_		"Invalid stack origin file name"

#define STK_INV_ORIG_LINE_		"Invalid stack origin line number"

#define STK_INV_SELF_PTR_		"Invalid stack self pointer"

#define STK_BASE_HASH_ERR_		"Invalid stack base hash"

#define STK_DATA_HASH_ERR_		"Invalid stack data hash"

//===================================================================

#define INV_FILE_NAME_             "Invalid file name"

#define FOPEN_ERROR_               "File can not be opened"

#define FCLOSE_ERROR_			   "File can not be closed"

#define FILE_OUTPUT_ERROR_         "File output error"

#define INV_ERROR_CODE_            "Invalid error code"

#define INV_STACK_PTR_			   "Invalid stack pointer"

#define RECALLOC_INV_NUM_OF_ELEMS_ "Invalid unmber of elements argument in recalloc"

#define RECALLOC_INV_POINTER_	   "Invalid pointer in recalloc argumnet"

#define RECALLOC_INV_SIZE_OF_ELEM_ "Invalid size of element in recalloc argument"

#define CANNOT_ALLOCATE_MEM_       "Memory allocation error"

#define CLEAR_MEM_CHECK_ERR_       "Memory cleanliness check is failed"

#define TEXT_PROCESSING_ERR_	   "Error during text processing"

#define LOG_REPORT_PARAMS_ERR_	   "Invalid arguments in log_report"

#define INV_TEXT_PTR_			   "Invalid text structure pointer"

#define TEXT_NO_STRINGS_		   "No strings in text structure"

#define NULL_STRING_TEXT_		   "Null string in text structure"

#define EMPTY_OPER_NAME_		   "Empty operation name"

#define INV_OPER_NAME_			   "Incorrect operation name"

#define STACK_CALC_DIV_ZERO_	   "Can not divide by zero"

#define INV_FILE_PTR_			   "Invalid FILE* pointer"

#define INV_HDR_PTR_			   "Invalid header structure pointer"

#define FWRITE_ERR_				   "Fwrite error"

#define INV_INSTRUCTION_STR_	   "Invalid instruction string in text structure"

#define CODE_FILE_SIZE_ERR_		   "Size of binary code file after assembling"\
								   "is incorrect"

#define EMPTY_OPERNAMEBUF_		   "Empty operation name buffer"

#define INV_PROCSTRUCT_PTR_        "Invalid performstruct structure pointer"

#define FREAD_ERR_				   "Frea returning value is not equal to size of file"

#define HDR_INV_SIGN_              "Invalid signature in header of code file"

#define HDR_INV_VERSION_		   "Versions of processor and assembled code are not equal"

#define HDR_INV_FILE_SIZE_         "Size of code file in header and real size are not equal"

#define HDR_INV_CMD_NUMBERS_	   "Invalid command numbers in header of code file"

#define INV_CODE_ARRAY_PTR_	       "Invalid pointer to the code array in procstruct structure"

#define PROC_INV_IP_               "Invalid instruction pointer " \
								   "(pointing to the outside of code array)"

#define PROC_INV_OPER_CODE_        "Invalid operation code in procstruct->code_array"

#define PROC_DIV_BY_ZERO_          "Processor error: number cannot be divided by zero"

#define FILE_INPUT_ERR_            "File input error"

#define INV_INSTR_PTR_			   "Instruction pointer do not point at the right place " \
								   "at the end of processing code array"

#define INV_COMMANDS_NUMBER_       "Number of instructions counted during compilation is" \
                                   "not equal to the number of instructions executed by " \
                                   "the processor"

#define INV_ARGUMENTS_NUMBER_      "Number of constant arguments counted during compilation " \
								   "is not equael to the number of used arguments during "\
								   "the processing"

#define NULL_STRING_               "Null string"

#define INV_REGISTER_NUMBER_       "Number of register is lower than zero or higher than number " \
								   "of registers available"

#define INV_REGISTER_ARGS_NUMBER_  "Number of register arguments counted during compilation "\
								   "is not equal to the number of used during processing " \
								   "register arguments"

#define INV_DISASMSTRUCT_PTR_      "Invalid disasmstruct structure pointer (NULL)"

#define DISASM_INV_OPER_CODE_      "Invalid operation code in code array during disassembling"

#define FTELL_INV_VALUE_           "Ftell return is -1, error occured"

#define ASMSTRUCT_INV_PTR_         "Null pointer to the assembler structure"

#define RAM_INV_ADDRESS_		   "Invalid ram address (number of element is more than ram capacity"

#define RAM_COMMANDS_INV_NUMBER_   "Number of ram using commands counted during compilation "\
								   "is not equal to the number counted during executing prohramm"

//===================================================================

enum stack_error_codes {

	STK_INVALSIZE          = 100,
	STK_NO_MEM             = 101,
	STK_OVRFLW             = 102,
	STK_UNDFLW             = 103,
	STK_NO_ELEMS_PUSHED	   = 104,
	STK_INV_NEW_SIZE       = 105,
	STK_CTOR_UNPREPARE     = 106,
	STK_SZ_LESS_CT         = 107,
	STK_UNKNOWN_RESZ_PARAM = 108,
	STK_INV_SZ_ELEM        = 109,
	STK_INV_CAPACITY       = 110,
	STK_INV_ELEM_CT        = 111,
	STK_CAPACITY_LESS_CT   = 112,
	STK_INV_FREE_DATA      = 113,
	STK_INV_DATA_PTR       = 114,
	STK_POISON_CHECK       = 115,
	STK_EMPTY_CHECK        = 116,
	STK_CANARY1_ERR        = 117,
	STK_CANARY2_ERR        = 118,
	STK_DATA_CANARY1_ERR   = 119,
	STK_DATA_CANARY2_ERR   = 120,
	STK_INV_ORIG_FUNC      = 121,
	STK_INV_ORIG_FILE      = 122,
	STK_INV_ORIG_LINE      = 123,
	STK_INV_SELF_PTR       = 124,
	STK_BASE_HASH_ERR      = 125,
	STK_DATA_HASH_ERR      = 126,
	STACK_CALC_DIV_ZERO    = 127,
	STK_CTOR_UNPREPARED    = 128,
};

enum global_error_codes {

	INV_FILE_NAME             = 300,
	FOPEN_ERROR               = 301,
	FILE_OUTPUT_ERROR         = 302,
	CLEAR_MEM_CHECK_ERR       = 303,
	INV_ERROR_CODE            = 304,
	INV_STACK_PTR             = 305,
	RECALLOC_INV_NUM_OF_ELEMS = 306,
	RECALLOC_INV_POINTER      = 307,
	RECALLOC_INV_SIZE_OF_ELEM = 308,
	CANNOT_ALLOCATE_MEM       = 309,
	TEXT_PROCESSING_ERR       = 310,
	LOG_REPORT_PARAMS_ERR     = 311,
	INV_TEXT_PTR              = 312,
	FCLOSE_ERROR              = 313,
	TEXT_NO_STRINGS           = 314,
	NULL_STRING_TEXT          = 315,
	EMPTY_OPER_NAME           = 316,
	INV_OPER_NAME             = 317,
	INV_FILE_PTR              = 318,
	INV_HDR_PTR               = 319,
	FWRITE_ERR                = 320,
	INV_INSTRUCTION_STR       = 321,
	CODE_FILE_SIZE_ERR        = 322,
	EMPTY_OPERNAMEBUF         = 323,
	INV_PROCSTRUCT_PTR        = 324,
	FREAD_ERR				  = 325,
	HDR_INV_SIGN              = 326,
	HDR_INV_VERSION           = 327,
	HDR_INV_FILE_SIZE         = 328,
	HDR_INV_CMD_NUMBERS       = 329,
	INV_CODE_ARRAY_PTR        = 330,
	PROC_INV_IP               = 331,
	PROC_INV_OPER_CODE        = 332,
	PROC_DIV_BY_ZERO          = 333,
	FILE_INPUT_ERR            = 334,
	INV_INSTR_PTR             = 335,
	INV_COMMANDS_NUMBER		  = 336,
	INV_ARGUMENTS_NUMBER      = 337,
	NULL_STRING               = 338,
	INV_REGISTER_NUMBER       = 339,
	INV_REGISTER_ARGS_NUMBER  = 340,
	INV_DISASMSTRUCT_PTR      = 341,
	DISASM_INV_OPER_CODE      = 342,
	FTELL_INV_VALUE           = 343,
	ASMSTRUCT_INV_PTR         = 344,
	ASMSTRUCT_EMPTY_CODE_ARR  = 345,
	ASMSTRUCT_INV_IP          = 346,
	RAM_INV_ADDRESS           = 347,
	RAM_COMMANDS_INV_NUMBER   = 348
};

enum assembler_errors {

};

enum general_errors {

};

enum processor_errors {

};
