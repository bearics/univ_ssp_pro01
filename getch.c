#include <stdio.h>
#include <termios.h>
#include "getch.h"
#include <unistd.h>

int getch(void)
{
	int ch;
	struct termios buf, save;
	tcgetattr(0, &save);
	buf = save;
	buf.c_lflag &= ~(ICANON|ECHO);	// 키보드 이벤트에 바로 반응
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &buf);	// 표준 입력으로 출력
	ch = getchar();
	tcsetattr(0, TCSANOW, &save);
	return ch;
}

STATE tab(char* input)	// tab 누를 때 나오는 메뉴 출력 함수
{
	if(getch() == '\t')	// 더블 텝일 경우에는 해당 경로에서 출력
	{
		printf("doube tab!");
		execl("/bin/ls", "ls", NULL);
	}
	else
		printf("sing tab!!");
	return INPUT;
}