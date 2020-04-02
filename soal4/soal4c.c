#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

int main(){
	pid_t child_id;
	int status;
	int fd[2];
	pipe(fd);
	
	if(pipe(fd) == -1) {
		die("pipe");
	}
	
	child_id = fork();
	if(child_id < 0) {
		exit(EXIT_FAILURE);
	}
	
	if(child_id == 0) {
		close(1);
		dup(fd[1]);
		close(fd[0]);
		char *list[] = {"ls", NULL};
		execv("/bin/ls", list);
	} else {
		while((wait(&status)) > 0);
		close(0);
		dup(fd[0]);
		close(fd[1]);
		char *count[] = {"wc", "-l", NULL};
		execv("/usr/bin/wc", count);
	}
}
		
 
