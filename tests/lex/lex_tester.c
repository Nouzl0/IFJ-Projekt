#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "../../src/error_handler.h"
#include "../../src/lex.h"

/*

TODO:
	Přidat testy na ?int ?float ?string
	testy na stringy podle "'
	pokrocile testy nazvy promenych a identifikatoru hlavne cisla a _
	pridat testy na maly pocet kratych tokenu idealne mensich nez BUFFER_SIZE
*/

void token_array_to_file(char* filename, tok_arr_t ta_ptr, int info_level){
	
	FILE* file = fopen(filename,"w");

	if(file == NULL){
		printf("Can't create test output files\n");
		return;
	}
	
	switch(info_level){
		case 1:
			for(int i = 0; i < ta_ptr.len; i++){		
				fprintf(file, "[%s]", token_enum_to_string(ta_ptr.elems[i].type));
			}
			break;
			
		case 2:
			for(int i = 0; i < ta_ptr.len; i++){		
				fprintf(file, "[%s,%d,%d]", token_enum_to_string(ta_ptr.elems[i].type),ta_ptr.elems[i].line,ta_ptr.elems[i].column);
			}
			break;
		
		case 3:
			for(int i = 0; i < ta_ptr.len; i++){		
				if (ta_ptr.elems[i].content == NULL){
					fprintf(file, "[%s,-]", token_enum_to_string(ta_ptr.elems[i].type));
				} else {
					fprintf(file, "[%s,%s]", token_enum_to_string(ta_ptr.elems[i].type), ta_ptr.elems[i].content);
				}	
			}
			break;
		default:
			for(int i = 0; i < ta_ptr.len; i++){		
				if (ta_ptr.elems[i].content == NULL){
					fprintf(file, "[%s,-,%d,%d]", token_enum_to_string(ta_ptr.elems[i].type),ta_ptr.elems[i].column,ta_ptr.elems[i].line);
				} else {
					fprintf(file, "[%s,%s,%d,%d]", token_enum_to_string(ta_ptr.elems[i].type), ta_ptr.elems[i].content,ta_ptr.elems[i].column,ta_ptr.elems[i].line);
				}	
			}
	}
	fclose(file);
}

void test_case(char* test_name, bool should_fail, int info_level, char* input_file, char* ref_file, char* output_file){
	
	errors_ctor(global_err_ptr);
	global_err_ptr->quiet_errors = true;
	
	
	tok_arr_t token_array;
	tok_arr_ctor(&token_array);
	
	lex_tokenize_file(&token_array, input_file);
	
	if(should_fail){
		
		if(global_err_ptr->error){
			printf("[\033[0;32mOK\033[0;37m] %s\n",test_name);
		} else {
			printf("[\033[0;31mFAIL\033[0;37m] %s: should fail but it didn't\n",test_name);
		}
	
	} else {
		
		if(global_err_ptr->error){
			printf("[\033[0;31mFAIL\033[0;37m] %s: should not fail but it did\n",test_name);
		} else {
			
			token_array_to_file(output_file, token_array, info_level);
			
			if(str_cmp_files(ref_file,output_file)){
				printf("[\033[0;31mFAIL\033[0;37m] %s: reference output and current output differs\n",test_name);
				str_ptrint_file(ref_file);
				str_ptrint_file(output_file);
				
			} else {
				printf("[\033[0;32mOK\033[0;37m] %s\n",test_name);
			}
			
		}
		
		
	}
	
	tok_arr_dtor(&token_array);
	
}

errors_t global_err;
errors_t* global_err_ptr = &global_err;

int main(){
	
	test_case(
		"should_recognize_all_symbols_with_spaces",
		false,
		1,
		"../lex/all_symbols_spaces.input",
		"../lex/all_symbols.out.ref",
		"../lex/all_symbols_spaces.out"
	);
	
	test_case(
		"should_recognize_all_symbols",
		false,
		1,
		"../lex/all_symbols.input",
		"../lex/all_symbols.out.ref",
		"../lex/all_symbols.out"
	);
	
	test_case(
		"should_recognize_all_keywords",
		false,
		1,
		"../lex/all_keywords.input",
		"../lex/all_keywords.out.ref",
		"../lex/all_keywords.out"
	);
	
	test_case(
		"should_read_numbers_type_and_content",
		false,
		3,
		"../lex/numbers.input",
		"../lex/numbers.out.ref",
		"../lex/numbers.out"
	);
	
	test_case(
		"should_read_variable_names",
		false,
		3,
		"../lex/variables.input",
		"../lex/variables.out.ref",
		"../lex/variables.out"
	);
	
	test_case(
		"should_read_identifier_names",
		false,
		3,
		"../lex/identifiers.input",
		"../lex/identifiers.out.ref",
		"../lex/identifiers.out"
	);
	
	test_case(
		"should_fail_no_string_end",
		true,
		1,
		"../lex/non_valid_string.input",
		"",
		""
	);
	
	test_case(
		"should_read_texts",
		false,
		3,
		"../lex/texts.input",
		"../lex/texts.out.ref",
		"../lex/texts.out"
	);
	
	test_case(
		"should_fail_no_comment_end",
		true,
		1,
		"../lex/non_valid_comment.input",
		"",
		""
	);
	
	test_case(
		"should_skip_comments",
		false,
		1,
		"../lex/comments.input",
		"../lex/comments.out.ref",
		"../lex/comments.out"
	);
	
	test_case(
		"should_return_right_lines",
		false,
		2,
		"../lex/lines.input",
		"../lex/lines.out.ref",
		"../lex/lines.out"
	);
	
	test_case(
		"should_return_right_columns",
		false,
		2,
		"../lex/columns.input",
		"../lex/columns.out.ref",
		"../lex/columns.out"
	);
	
	test_case(
		"should_correctly_read_complex_statement",
		false,
		1,
		"../lex/complex_statement.input",
		"../lex/complex_statement.out.ref",
		"../lex/complex_statement.out"
	);
	
	test_case(
		"should_correctly_read_number_statement",
		false,
		3,
		"../lex/numbers_statement.input",
		"../lex/numbers_statement.out.ref",
		"../lex/numbers_statement.out"
	);
	
	return 0;
}