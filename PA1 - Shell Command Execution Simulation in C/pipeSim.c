// Mert Kilicaslan - Programming Assignment 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>


int main(){

	printf("I'm SHELL process, with PID:%d - Main command is: man ping | grep -A 5 -e -A > output.txt\n", (int) getpid());
	
	int fd[2];
	// Pipe failed
	if(pipe(fd) < 0){
		perror("Pipe");
		exit(1);
	}
	
	int rc1 = fork(); 
	// Fork 1 failed
	if(rc1 < 0){
		perror("Fork");
		exit(1);
	}
	
	// First child (man command)
	else if(rc1 == 0){
		
		printf("I'm MAN process, with PID:%d - My command is: ping\n", (int) getpid());
		
		if(dup2(fd[1], STDOUT_FILENO) < 0){
			perror("Dup2");
			exit(1);
		}
		
		// close read end
		close(fd[0]);
		
		// ping: command variable
		char* args[] = {"man", "ping", NULL};
		execvp(args[0], args);
	
	}
		
	waitpid(rc1, NULL, 0); // parent waits for the first child
	
	int rc2 = fork();
	// Fork 2 failed
	if(rc2 < 0){
		perror("Fork");
		exit(1);
	}
	
	// Second child (grep command)
	else if(rc2 == 0){
		
		printf("I'm GREP process, with PID:%d - My command is: -A 5 -e -A > output.txt\n", (int) getpid());
	
		if(dup2(fd[0], STDIN_FILENO) < 0){
			perror("Dup2");
			exit(1);
		}
		
		// redirecting output from std_output to new file output.txt		
		int file = open("output.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
		if(dup2(file, STDOUT_FILENO) < 0){
			perror("Dup2");
			exit(1);
		}	
		
		// close write end
		close(fd[1]);
		
		// -A 5 :enforce to grep next five lines
		// -e : protect next pattern that begins with '-' for accepting as string 
		// -A : selected option
		char* args[] = {"grep", "-A", "5", "-e", "-A", NULL};
		execvp(args[0], args);	
	
	}
	
	// Parent	
	close(fd[1]);
	close(fd[0]);

	waitpid(rc2, NULL, 0); // parent waits for second child
	
	printf("I'm SHELL process, with PID:%d - execution is completed, you can find the results in output.txt\n", (int) getpid());
	
	return 0;
}
