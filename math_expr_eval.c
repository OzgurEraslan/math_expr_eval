#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "math_expr_eval.h"

typedef struct parser_s
{
	const char* cursor;
	const char* error_string;
	double parsed_number;
	double stack[64];
	int stack_top;
	char token;
} parser_t;

typedef struct constant_s
{
	const char* keyword;
	double value;
} constant_t;

constant_t constants[] =
{
	{ "pi", 3.14159265358979323846 },
	{ "tau", 3.14159265358979323846 * 2.0 },
	{ "e", 2.71828182845904523536 },
	{ NULL, 0.0 }
};

void tokenize(parser_t* parser)
{
	char c = *parser->cursor++;
	while (c != 0)
	{
		switch (c)
		{
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			c = *parser->cursor++;
			continue;
		case '(':
		case ')':
		case '*':
		case '/':
		case '%':
		case '+':
		case '-':
		case '^':
			parser->token = c;
			return;
		default:
			if ((c >= '0' && c <= '9') || c == '.')
			{
				int i = 0;
				char tmp[64];
				do
				{
					tmp[i++] = c;
					c = *parser->cursor++;
				} while (((c >= '0' && c <= '9') || c == '.') && c != 0);
				parser->cursor--;
				tmp[i] = '\0';
				parser->parsed_number = strtod(tmp, NULL);
				parser->token = '#';
				return;
			}
			else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
			{
				int i = 0;
				char tmp[64];
				do
				{
					tmp[i++] = c;
					c = *parser->cursor++;
				} while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
				parser->cursor--;
				tmp[i] = '\0';

				int keyword_index = 0;
				constant_t constant = constants[keyword_index++];
				while (constant.keyword)
				{
					if (strcmp(constant.keyword, tmp) == 0)
					{
						parser->parsed_number = constant.value;
						parser->token = '#';
						return;
					}
					constant = constants[keyword_index++];
				}
			}

			parser->token = 0;
			parser->error_string = "Unknown token";
			return;
		}
	}

	parser->token = 0;
}

void push_stack(parser_t* parser, double d) 
{
	if (parser->stack_top < 64) 
		parser->stack[parser->stack_top++] = d; 
	else 
		parser->error_string = "Stack overflow"; 
}

double pop_stack(parser_t* parser) 
{ 
	if (parser->stack_top > 0)
		return parser->stack[--parser->stack_top];
	else
		return 0.0;
}

typedef void math_expr(parser_t*);
void binary_op(parser_t* parser, math_expr* next_expr)
{
	char token = parser->token;
	tokenize(parser);
	next_expr(parser);
	double r = pop_stack(parser);
	double l = pop_stack(parser);
	switch (token)
	{
	case '+': push_stack(parser, l + r); break;
	case '-': push_stack(parser, l - r); break;
	case '*': push_stack(parser, l * r); break;
	case '/': push_stack(parser, l / r); break;
	case '^': push_stack(parser, pow(l, r)); break;
	case '%': push_stack(parser, modfl(l, &r)); break;
	}
}

void term_expr(parser_t* parser);

void prefix_expr(parser_t* parser)
{
	if (parser->token == '-')
	{
		tokenize(parser);
		if (parser->token == '#') push_stack(parser, -parser->parsed_number);
		else
		{
			term_expr(parser);
			push_stack(parser, -(pop_stack(parser)));
		}
	}
	else if (parser->token == '#')
	{
		push_stack(parser, parser->parsed_number);
		tokenize(parser);
	}
	else if (parser->token == '(')
	{
		tokenize(parser);
		term_expr(parser);
		if (parser->token != ')') parser->error_string = "Expected (";
		tokenize(parser);
	}
	else
	{
		parser->error_string = "Unexpected token";
	}
}

void pow_expr(parser_t* parser)
{
	prefix_expr(parser);
	while (1)
	{
		if (parser->token == '^')
			binary_op(parser, prefix_expr);
		else
			return;
	}
}

void factor_expr(parser_t* parser)
{
	pow_expr(parser);
	while (1)
	{
		if (parser->token == '*' || parser->token == '/')
			binary_op(parser, pow_expr);
		else
			return;
	}
}

void term_expr(parser_t* parser)
{
	factor_expr(parser);
	while (1)
	{
		if (parser->token == '+' || parser->token == '-')
			binary_op(parser, factor_expr);
		else
			return;
	}
}

double math_expr_eval(const char* input, const char** error_str)
{
	parser_t parser;
	parser.cursor = input;
	parser.parsed_number = 0.0;
	parser.stack_top = 0;
	parser.error_string = NULL;
	tokenize(&parser);
	while (parser.token != 0 && parser.error_string == NULL)
	{
		term_expr(&parser);
	}

	if (parser.error_string)
	{
		if (error_str) 
			*error_str = parser.error_string;
		return 0.0;
	}
	return parser.stack[0];
}