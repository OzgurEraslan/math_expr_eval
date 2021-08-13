# math_expr_eval
Mathematical expression evaluator. \
Evaluates simple mathematical expressions provided as string like "5 + 4 - tau - 4 / 2 ^ 3 + (pi + (6 + 1) * e)".

### Example
```C++
#include <stdio.h>
#include "math_expr_eval.h"

int main(int argc, const char** argv)
{
	const char* test_string = "5 + 4 - tau - 4 / 2 ^ 3 + (pi + (6 + 1) * e)";
	const char* error_str = NULL;
	double result = math_expr_eval(test_string, &error_str);
	if(error_str)
	{
		printf("Error: %s!\n", error_str);
		return 1;
	}
	else
	{
		printf("%s = %f\n", test_string, result);
	}
	return 0;
}
```