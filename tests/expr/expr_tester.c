#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "../../src/error_handler.h"
#include "../../src/lex.h"
#include "../../src/statement_parser.h"

void expr_test_print(FILE* f, ptree_item_t* expr_node){
	if (!expr_node){
		fprintf(f,"null");
		return;
	}
	fprintf(f,"{");
	
	fprintf(f,"\"type\": \"%s\",",token_enum_to_string(expr_node->token.type));
	if(expr_node->token.content != NULL){
		fprintf(f,"\"content\": \"%s\",",expr_node->token.content);
	}
	if(expr_node->is_terminal){
		fprintf(f,"\"is_terminal\": true");
	} else {
		fprintf(f,"\"is_terminal\": false");
	}
	if(expr_node->params_len){
		fprintf(f,",\"params\":[");
		for (int i = 0; i < expr_node->params_len; i++){
			expr_test_print(f,expr_node->params[i]);
			if(i != expr_node->params_len-1){
				fprintf(f,",");
			}
		}
		fprintf(f,"]");
	}
	
	if (expr_node->left){
		fprintf(f,",\"left\": ");
		expr_test_print(f,expr_node->left);
	}
	
	if (expr_node->right){
		fprintf(f,",\"right\": ");
		expr_test_print(f,expr_node->right);
	}
	
	fprintf(f,"}");
}

void expr_to_file(char* output_file, ptree_item_t* expr_root){
	FILE* file = fopen(output_file,"w");

	if(file == NULL){
		printf("Can't create test output files\n");
		return;
	}
	
	expr_test_print(file, expr_root);
	fclose(file);
}

void test_case(char* test_name, bool should_fail, char* input_file, char* ref_file, char* output_file){
	
	errors_ctor(global_err_ptr);
	global_err_ptr->quiet_errors = true;
	
	
	tok_arr_t token_array;
	tok_arr_ctor(&token_array);
	
	lex_tokenize_file(&token_array, input_file);
	
	ptree_item_t* expr_ptr =  expr_parse(&token_array, SEMICOLON);
	
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
			expr_to_file(output_file, expr_ptr);
			
			
			if(str_cmp_files(ref_file,output_file)){
				printf("[\033[0;31mFAIL\033[0;37m] %s: reference output and current output differs\n",test_name);
				str_ptrint_file(ref_file);
				str_ptrint_file(output_file);
				
			} else {
				printf("[\033[0;32mOK\033[0;37m] %s\n",test_name);
			}
			
		}
	}
	
	if(expr_ptr){
		ptree_dtor(expr_ptr);
	}
	
	tok_arr_dtor(&token_array);
	
}

errors_t global_err;
errors_t* global_err_ptr = &global_err;

int main(){
	
	test_case(
		"only_number",
		false,
		"../expr/number.input",
		"../expr/number.out.ref",
		"../expr/number.out"
	);
	
	test_case(
		"expression_without_terminating_character",
		true,
		"../expr/without_end.input",
		"",
		""
	);
	
	test_case(
		"operator_with_only_one_operand",
		true,
		"../expr/one_operand.input",
		"",
		""
	);
	
	test_case(
		"operands_without_operator",
		true,
		"../expr/without_operator.input",
		"",
		""
	);
	
	test_case(
		"valid_parentheses_around_valid_expression",
		false,
		"../expr/parens.input",
		"../expr/parens.out.ref",
		"../expr/parens.out"
	);
	
	test_case(
		"missing_right_parenthesis_after_valid_expression",
		true,
		"../expr/missing_r_paren.input",
		"",
		""
	);
	
	test_case(
		"missing_left_parenthesis_before_valid_expression",
		true,
		"../expr/missing_l_paren.input",
		"",
		""
	);
	
	test_case(
		"empty_parentheses_in_valid_expression",
		true,
		"../expr/empty_parens.input",
		"",
		""
	);
	
	test_case(
		"valid_expression_in_double_parentheses",
		false,
		"../expr/double_parentheses.input",
		"../expr/double_parentheses.out.ref",
		"../expr/double_parentheses.out"
	);
	
	test_case(
		"invalid_expression_in_parentheses",
		true,
		"../expr/invalid_parentheses.input",
		"",
		""
	);
	
	test_case(
		"function_name_without_parameters",
		true,
		"../expr/only_identifier.input",
		"",
		""
	);
	
	test_case(
		"valid_function",
		false,
		"../expr/valid_function.input",
		"../expr/valid_function.out.ref",
		"../expr/valid_function.out"
	);
	
	test_case(
		"valid_function_one_parameter",
		false,
		"../expr/valid_function_param.input",
		"../expr/valid_function_param.out.ref",
		"../expr/valid_function_param.out"
	);
	
	
	test_case(
		"valid_function_two_parameters",
		false,
		"../expr/valid_function_params.input",
		"../expr/valid_function_params.out.ref",
		"../expr/valid_function_params.out"
	);
	
	
	test_case(
		"ivalid_function_first_parameter",
		true,
		"../expr/invalid_function_param.input",
		"",
		""
	);
	
	test_case(
		"ivalid_function_second_parameter",
		true,
		"../expr/invalid_function_params.input",
		"",
		""
	);
	
	test_case(
		"function_within_function",
		false,
		"../expr/func_in_func.input",
		"../expr/func_in_func.out.ref",
		"../expr/func_in_func.out"
	);
	
	test_case(
		"advanceded_precendence",
		false,
		"../expr/advanceded_prec.input",
		"../expr/advanceded_prec.out.ref",
		"../expr/advanceded_prec.out"
	);
	
	/*
	
	*/
	return 0;
}