#include <stdio.h>
#include <termios.h>
#include "getch.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int getch(void)
{
	int ch;
	int tb =1 ;// 1이면 싱글 텝, 2이면 더블 텝 
	int stat;
	pid_t pid;

	struct termios buf, save;
	tcgetattr(0, &save);
	buf = save;
	buf.c_lflag &= ~(ICANON|ECHO);	// 키보드 이벤트에 바로 반응
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &buf);	// 표준 입력으로 출력
	while((ch = getchar()) == '\t' || ch == 8)
	{
		if(ch == 8)
		{	// 벡스페이스 누르면 글자 삭제 하는 방법
			tcsetattr(0, TCSANOW, &save);
			printf("\b \b");
			input[strlen(input)-1]=0x00;
			tcsetattr(0, TCSANOW, &buf);
		}
		else
		{
			switch(pid=fork())
			{
				case 0:	// for child process
					if(getchar() == '\t')
					{
						tab(2);	// 더블텝 실행
						exit(1);
					}
					break;
				default:	// for parent process
					sleep(DOUBLE_TIME);
					if( 0 == waitpid(pid, &stat, WNOHANG))
					{
						kill(pid, SIGKILL);
						tab(1);	// 싱글텝 실행	
					}
					break;
			}
		}

	}

	tcsetattr(0, TCSANOW, &save);
	return ch;
}

STATE tab(int num)	// tab 누를 때 나오는 메뉴 출력 함수
{
	char temp;
	char path[30];	// 현재 명령어가 원하는 경로 저장 함수
	int pid;


	// 현재 경로를 path 변수에 저장
	int max=strlen(input);
	int i=0;
	bzero(path, 30);
	while(input[max - ++i] != ' ') {}
	i--;
	for(int j=0; j<i; j++)
		path[j] = input[max - (i-j)];



	if(num == 1){
		// int fd[2];
		// int bakfd[2];
		// int childpid[2];

		// dup2(STDIN_FILENO, bakfd[0]);
		// dup2(STDOUT_FILENO, bakfd[1]);

		// if((pipe(fd) == -1) || ((childpid[0] = fork()) == -1)) {
		// 	perror("Failed to setup pipeline");
		// 	return 1;
		// }
		// if(childpid[0] == 0) {
		// 	if(dup2(fd[1], STDOUT_FILENO) == -1)
		// 		perror("Failed to redirect stdout of ls");
		// 	else if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
		// 		perror("Failed to close extra pipe descriptors on ls");
		// 	else {
		// 		execl("/bin/ls", "ls", NULL);
		// 		perror("Failed to exec ls");
		// 	}
		// 	return 1;
		// }
		// wait(NULL);
		// childpid[1] = fork();
		// if(childpid[1] == 0 )
		// {
		// 	if(dup2(fd[0], STDIN_FILENO) == -1)
		// 		perror("Failed to redirect stdin of sort");
		// 	else if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
		// 		perror("Failed to close extra pipe file descriptors on sort");
		// 	else {
		// 		execl("/bin/grep", "grep", path, NULL);
		// 	}
		// 	return 1;
		// }
		// wait(NULL);
		// dup2(bakfd[0], STDIN_FILENO);
		// dup2(bakfd[1], STDOUT_FILENO);
	}
	else{
		switch(pid=fork())
		{
			case 0:	// for child process
				puts("");
				execl("/bin/ls", "ls", path, NULL);
				exit(1);
			default:	// for parent process
				wait(NULL);
				break;
		}
		printf("User Shell >> %s", input);
	}

	return INPUT;
}

