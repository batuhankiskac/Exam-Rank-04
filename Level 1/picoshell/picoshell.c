#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int    picoshell(char **cmds[]) {
	int num_cmds;

	while (!cmds[num_cmds])
		num_cmds++;
	if (num_cmds == 0)
		return (0);

	int pipe_fd[2];
	int	prev_fd = -1;

	for (int i = 0; i < num_cmds; i++) {
		if (i < num_cmds - 1) {
			if (pipe(pipe_fd) == -1)
				return (1);
		}

		pid_t pid = fork();
		if (pid == -1)
			return (1);

		if (pid == 0) {
			if (prev_fd != -1) {
				dup2(prev_fd, STDIN_FILENO);
				close(prev_fd);
			}
			if (i < num_cmds -1 ) {
				dup2(pipe_fd[1], STDOUT_FILENO);
				close(pipe_fd[0]);
				close(pipe_fd[1]);
			}
			execvp(cmds[i][0], cmds[i]);
			perror("execvp");
			exit(1);
		}
		if (prev_fd != -1)
			close(prev_fd);
		if (i < num_cmds - 1) {
			close(pipe_fd[1]);
			prev_fd = pipe_fd[0];
		}
	}
	for (int i = 0; i < num_cmds; i++) {
		wait(NULL);
	}
	return (0);
}
