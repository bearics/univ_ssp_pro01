#include "getch.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>

int copyFile(int sour, int dest);
int token(int pos, char* left, char* right);
void getInput();
void parse(int arg[][2]);
void execute(char* cmd,int in, int out, int sleep);

int main(int argc, char* argv[])
{
	int pid;	// 자식 프로세스 pid
	int fd[2];	// 프로세스 간의 통신을 위한 파이프
	int file;	// 파일 저장 용 FD
	int cntcmd;	// 명령어의 개수

	int temp;
	int pos; // 기호 오프셋 저장

	char left[100];	//왼쪽 명령어 
	char right[100];	//오른쪽 명령어 

	int arg[10][2];	// 명령어 개수 및 유형(|, <, >) 저장

	while(1)	
	{	// 사용자로부터 명령어 입력을 받는다.
		getInput();
		bzero(arg, sizeof(arg));
		parse(arg);
		int cnt = 0;	// 명령어 개수
		int infp = open("intmp", O_RDWR | O_CREAT | O_TRUNC, 0644);
		int outfp = open("outtmp", O_RDWR | O_CREAT | O_TRUNC, 0644);
		while(arg[cnt][0] > 0 )
		{
			token(arg[cnt][0], left, right);
			printf("left %s, right %s\n", left, right);
			//execute(left, fd[1], STDOUT_FILENO);
			//execute(right, fd[0], STDIN_FILENO);
			switch((STATE)arg[cnt][1])
			{
				case PIPE:
					printf("PIPE~~~\n");
					copyFile(outfp, infp);
					execute(left, infp, outfp, 0);
					break;
				case LT:	// ls -al < file1
					printf("LT~~~\n");
					file = open(left, O_RDWR | O_CREAT | O_TRUNC, 0644);
					execute(right, outfp, file, 0);
					close(file);
					break;
				case GT:
					printf("GT~~~\n");
					file = open(right, O_RDWR | O_CREAT | O_TRUNC, 0644);
					execute(left, outfp, file, 0);
					close(file);
					break;
				case AMP:
					printf("AMP~~~\n");
					execute(left, outfp, file, 1);
					cnt++;
					break;
				case END:
					printf("END~~~\n");
					execute(left, outfp, 1, 0);
					break;
			}
			cnt++;
			sleep(0.5);
			close(infp);
			close(outfp);
		}

	}
	return 0;
}

void parse(int arg[][2])
{
	int i=0;
	int cnt=0;	// 명령어 개수
	char c;
	while((c = input[i++]))
	{
		switch(c)
		{
			case '|':
				arg[cnt][0]=(i-1);
				arg[cnt++][1] = (int)PIPE;
				continue;
			case '<':
				arg[cnt][0]=(i-1);
				arg[cnt++][1] = (int)LT;
				continue;
			case '>':
				arg[cnt][0]=(i-1);
				arg[cnt++][1] = (int)GT;
				continue;
			case '&':
				arg[cnt][0]=(i-1);
				arg[cnt++][1] = (int)AMP;
				continue;
			default:
				continue;
		}
	}
	arg[cnt][0]=(i);
	arg[cnt++][1] = (int)END;
}

void getInput()
{	// 사용자로부터 명령어를 입력 받는다.
	char* p = input; // pointer to input
	int i= 0;
	int c;	// temporarily save input char
	bzero(input, MAX_INPUT);	// 쓰래기 값들 모두 지우기
	printf("User Shell >> ");
	while((c = getch()) != 10)	// until enter input
	{
		printf("%c", c);
		p[strlen(p)] = (char)c;
		continue;
	}
	printf("\ninput : %s\n", p);
}

void execute(char* cmd,int in, int out, int sleep)
{	// ls -al 과 같이 명령어를 받으면 띄어쓰기 단위로 잘라서 자식 프로세스로 실행한다.
	int fd[2];	// 자식 프로세스와 통신하기 위한 파이프
	char* cmdTok[10];
	bzero(cmdTok, 10*sizeof(char*));
	cmdTok[0] = strtok(cmd, " ");	  // " " 공백 문자를 기준으로 문자열을 자름, 포인터 반환
	int i=0;

	while (cmdTok[i++] != NULL)			   // 자른 문자열이 나오지 않을 때까지 반복
	{
		cmdTok[i] = strtok(NULL, " ");	  // 다음 문자열을 잘라서 포인터를 반환
	}
	cmdTok[i] = NULL;

	int pid;

			if(dup2(in, STDIN_FILENO) == -1)
				perror("Failed to redirect stdin");
			if(dup2(out, STDOUT_FILENO) == -1)	// 
				perror("Failed to redirect stdout");

	switch(pid = fork())
	{
		case 0:

			execvp(cmdTok[0], cmdTok);
			if( sleep == 0)
				exit(1);
			break;
		default:
			if(sleep == 0)
				wait(NULL);
			dup2(2,1);	// 표준 입출력 원상 복구 
			dup2(2,0);	// 표준 입출력 원상 복구 
			break;
	}
}

int token(int pos, char* left, char* right)
{
	bzero(left, 100);
	bzero(right, 100);
	int i=0, j=0, temp;

	pos--;
	while(!(input[pos-i] == '>' || input[pos-i] == '<' || input[pos-i] == '|' || input[pos-i] == '&' ) && ((pos-i) >=0))
	{i++;}
	temp = --i;
	for(j=0;j<temp;j++){
		left[j] = input[pos-(i--)];
	}
	if( pos == strlen(input) )
		right = NULL;	// 명령어 의 오른쪽이 없는 상태 예외 처리
	else
	{
		i=1; j=0;
		pos += 2;
		while(!(input[pos+i] == '>' || input[pos+i] == '<' || input[pos+i] == '|' || input[pos+i] == '&' ) && ((pos+i)<= strlen(input)))
		{
			right[j++] = input[pos+i++];
		}	
	}
}

int copyFile(int sour, int dest)
{
	char buff[1024];

	while(read(sour, buff, 1024) > 0)
	{
		write(dest, buff, strlen(buff));
	}
}

