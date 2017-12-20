#ifndef _GETCH_H_
#define _GETCH_H_

#define DOUBLE_TIME 1	// 더블 텝 인식 시간
#define MAX_INPUT 100

char input[MAX_INPUT];	// 사용자가 입력하는 명령어

typedef enum{
	END, INPUT, PIPE, AMP, LT, GT, TAB
}STATE;

int getch(void);
STATE tab(int num);

#endif // _GETCH_H_