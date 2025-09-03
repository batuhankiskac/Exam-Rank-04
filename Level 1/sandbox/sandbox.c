#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>

static volatile sig_atomic_t g_timeout_flag = 0;

void handle_alarm(int sig) {
	(void)sig;
	g_timeout_flag = 1;
}

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose) {
	struct sigaction sa;
	pid_t pid;
	int status;

	sa.sa_handler = handle_alarm;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGALRM, &sa, NULL) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0) {
		f();
		exit(0);
	}
	alarm(timeout);
	if (waitpid(pid, &status, NULL) == -1) {
		if (g_timeout_flag) {
			kill(pid, SIGKILL);
			waitpid(pid, &status, NULL);
			if (verbose)
				printf("Bad function: timed out after %u seconds\n", timeout);
			return (0);
		}
		return (-1);
	}
	alarm(0);
	if (WIFEXITED(status)) {
		int exit_code = WEXITSTATUS(status);
		if (exit_code == 0) {
			if (verbose)
				printf("Nice function!\n");
			return (1);
		} else {
			if (verbose)
				printf("Bad function: exited with code %d\n", exit_code);
			return (0);
		}
	} else if (WIFSIGNALED(status)) {
		if (verbose)
			printf("Bad function: %s\n", strsignal(WTERMSIG(status)));
		return (0);
	}
	return (-1);
}
