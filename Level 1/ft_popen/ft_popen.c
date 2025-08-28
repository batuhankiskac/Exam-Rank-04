#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int	ft_popen(const char *file, char *const argv[], char type)
{
	int pipe_fd[2];
	pid_t pid;

	if (type != 'r' && type != 'w')
		return (-1);
	if (pipe(pipe_fd) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
	{
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return (-1);
	}
	if (pid == 0)
	{
		if (type == 'r')
		{
			close(pipe_fd[0]);
			if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
				exit(EXIT_FAILURE);
			close(pipe_fd[1]);
		}
		else
		{
			close(pipe_fd[1]);
			if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
				exit(EXIT_FAILURE);
			close(pipe_fd[0]);
		}
		execvp(file, argv);
		exit(EXIT_FAILURE);
	}
	if (type == 'r')
	{
		close(pipe_fd[1]);
		return (pipe_fd[0]);
	}
	if (type == 'w')
	{
		close(pipe_fd[0]);
		return (pipe_fd[1]);
	}
	return (-1);
}
