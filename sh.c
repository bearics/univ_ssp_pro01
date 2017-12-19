#include "getch.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_INPUT 100



STATE getInput(char* input);
void autoComplete(void);
int getKey();	// 실시간 입력 함수

int main(int argc, char* argv[])
{
	pid_t childpid;
	int fd[2];

	char input[MAX_INPUT];	// 사용자가 입력하는 명령어
	int temp ;
	int sn = 0; // spcae number 

	STATE state = END;


	while(1)	// input
	{	// 입력 받은 명령어의 상태에 따라 swtch-case 구문으로 나눈다.
		switch(state)
		{
			case INPUT:
				state = getInput(input);
				continue;
			case LT:
				state = INPUT;
				continue;
			case GT:
				state = INPUT;
				continue;
			case PIPE:
				state = INPUT;
				continue;
			case TAB:
				state = tab(input);
				continue;
			case AMP:
				state = INPUT;
				continue;
			case END:
				state = INPUT;
				fputs("User Shell >> ", stdout);
				break;

		}
	}
	return 0;
}



STATE getInput(char* input)
{	// 한글자씩 받으면서 현재 상태를 반환한다.
	char* p = input; // pointer to input
	int c;	// temporarily save input char

	while((c = getch()) != 10)	// until enter input
	{
		switch(c)
		{
			case '|':
				printf("|\n");
				return PIPE;
			case '<':
				printf("<\n");
				return LT;
			case '>':
				printf(">\n");
				return GT;
			case '&':
				printf("&\n");
				return AMP;
			case '\t':
				return TAB;
			default:
				printf("%c", c);
				*p++ = c;
				continue;
		}
	}

	printf("input : %s\n", p);

}
