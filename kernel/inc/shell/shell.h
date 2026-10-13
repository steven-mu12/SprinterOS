#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdint.h>

#define SHELL_LINE_MAX  128
#define SHELL_PROMPT    "sprinter > "
#define LOGIN_PROMPT    "login > "

struct shell_cmd {
	char* name;
	char* help;
	void (*run)(char* args);
};

void shell(void* args);
void shell_run(char* line);

#endif
