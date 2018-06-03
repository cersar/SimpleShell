#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#define MAX_LINE 80 /* The maximum length command */
#define MAX_HISTORY_NUM 10

void freeSpace(char** array, int cnt);
int min(int a, int b);
int splitCommand(char *commandLine, char ** args);

int main() {
	char* args[MAX_LINE / 2 + 1]; /* command line arguments */
	char history[MAX_HISTORY_NUM][100] = { 0 };
	int history_cnt = 0;
	int should_run = 1; /* flag to determine when to exit program */
	while (should_run) {
		printf("osh>");
		fflush(stdout);

		/**
		 * After reading user input, the steps are:
		 * (1) fork a child process using fork()
		 * (2) the child process will invoke execvp()
		 * (3) if command included &, parent will invoke wait()
		 */

		char commandLine[100], tmp[100];
		int should_wait = 1;
		int is_history_task=0;
		gets(commandLine);
		strcpy(tmp, commandLine);
		int cnt = splitCommand(tmp, args);

		if (cnt == 1) {
			if (strcmp(args[0], "history") == 0) {
				printf("显示历史任务：\n");
				for (int i = 0; i < min(10, history_cnt); ++i) {
					printf("%s\n", history[(history_cnt - 1 - i) % MAX_HISTORY_NUM]);
				}
				continue;
			} else if (strcmp(args[0], "quit") == 0) {
				should_run = 0;
				continue;
			} else if (strlen(args[0]) == 2 && args[0][0] == '!') {
				if (isdigit(args[0][1]) || args[0][1] == '!') {
					int index = (args[0][1] == '!') ? 0 : args[0][1] - '0';
					if (index + 1 <= min(history_cnt, MAX_HISTORY_NUM)) {
						int storeIndex = (history_cnt - 1 - index) % MAX_HISTORY_NUM;
						printf("执行倒数第%d历史任务: %s\n", index + 1,
								history[storeIndex]);
						strcpy(tmp, history[storeIndex]);
						cnt = splitCommand(tmp, args);
						is_history_task=1;
					} else {
						printf("输入参数错误：不存在倒数第%d次任务！", index + 1);
						continue;
					}

				}
			}
		}
		if(strcmp(args[cnt-1],"&")==0){
			should_wait=0;
			args[cnt-1] = NULL;
		}

		pid_t pid;
		/* fork a child process */
		pid = fork();
		if (pid < 0) { /* error occurred */
			fprintf(stderr, "Fork Failed");
			return 1;
		} else if (pid == 0) { /* child process */
			execvp(args[0], args);
			return 0;
		} else { /* parent process */
			/* parent will wait for the child to complete */
			if(should_wait){
				wait(NULL);
				printf("Child Complete: pid = %d\n", pid);
			}
			if(!is_history_task){
				strcpy(history[(history_cnt++) % MAX_HISTORY_NUM], commandLine);
			}

			printf("return to commandline ...\n");
		}

	}
	return 0;
}

int min(int a, int b) {
	return a < b ? a : b;
}

int splitCommand(char *commandLine, char ** args) {

	int cnt = 0;

	args[cnt] = strtok(commandLine, " ");
	while (args[cnt] != NULL) {
		args[++cnt] = strtok(NULL, " ");
	}
	args[cnt] = NULL;
	return cnt;
}
