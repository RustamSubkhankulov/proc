#pragma once

#define DEF_ERR(code, name, string) 								 \
																	 \
	name  = code,													 \

enum errors {

	#include "../text_files/errors.txt"
};

#undef DEF_ERR

