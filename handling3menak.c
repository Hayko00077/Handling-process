#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>

int *count;

void lag(const char* str) {
	while (*str) {
		usleep(30000);
		printf("%c", *str++);
		fflush(stdout);
	}
}

void handler(int sig) {
	(*count)++;	
}

void alarm_handler(int sig) {
	printf("\n");
	lag("SIGUSR1 signal current count ");
	printf("%d \n", *count);
	return;
}


int main() {
	char str[5];
	count = (int*)mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	pid_t pid = fork();
	
	if (pid < 0) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (pid == 0) {
		signal(SIGUSR1,handler);
		pause();
		while(1);
	} else {
		int status;
		
		while (1) {
			usleep(30000);
			signal(SIGALRM,alarm_handler);
			raise(SIGALRM);
		
			printf("\n");
			lag("Do you want to kill your process? (Yes/No)\n");
			scanf("%s", str);
			if (!strcmp(str, "Yes")) {
				kill(pid,SIGKILL);
				printf("\n");
				lag("Child process killed\n");
				exit(EXIT_SUCCESS);


			} else if (!strcmp(str, "No")) {
				int sig_count = 0;
				printf("\n");
				lag("How many time send signal SIGUSR1?\n");
				scanf("%d", &sig_count);
				for(int i = 0; i < sig_count; ++i) {
					kill(pid,SIGUSR1);
					usleep(30000);
				}


			  } else {
				printf("\n");
				lag("<<<< Input correct string >>>>\n");
		    	}
			 
		}
	}
}
