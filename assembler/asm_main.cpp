#include "assembler.h"

#define $ if (err_val == -1) return -1;

int main(int argc, char* argv[]) {
	
	int err_val = 0;
	struct Text text = { 0 };

	char* buf = text_init(argv[1], &text);
	text_remove_comments(&text); $

	struct AsmStruct asmstruct = { 0 };

	err_val = convert_operations_to_binary(&text, &asmstruct); $
	err_val = header_make(&asmstruct); $                               ///добавить проверку информации о метках

	FILE* code_file = open_code_file(argv[2], "wb"); 
	file_ptr_check(code_file);

	err_val = write_code(&asmstruct, code_file); $
	err_val = close_code_file(code_file); $

	text_clear_mem(&text, buf);

	return 0;
}