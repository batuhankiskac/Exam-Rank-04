#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int	ft_popen(const char *file, char *const argv[], char type) {
	int pipe_fd[2];
	pid_t pid;

	if (type != 'r' && type != 'w')
		return (-1);
	pipe(pipe_fd);
	pid = fork();
	if (pid == 0) {
		if (type == 'r') {
			close(pipe_fd[0]);
			dup2(pipe_fd[1], STDOUT_FILENO);
			close(pipe_fd[1]);
		} else {
			close(pipe_fd[1]);
			dup2(pipe_fd[0], STDIN_FILENO);
			close(pipe_fd[0]);
		}
		execvp(file, argv);
		exit(EXIT_FAILURE);
	}
	if (type == 'r') {
		close(pipe_fd[1]);
		return (pipe_fd[0]);
	} else {
		close(pipe_fd[0]);
		return (pipe_fd[1]);
	}
	return (-1);
}
