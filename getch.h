#ifndef _GETCH_H_
#define _GETCH_H_

typedef enum{
	INPUT, PIPE, AMP, LT, GT, TAB, END
}STATE;

int getch(void);
STATE tab(char* input);

#endif // _GETCH_H_