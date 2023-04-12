/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segarcia <segarcia@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/11 12:28:39 by segarcia          #+#    #+#             */
/*   Updated: 2023/04/12 11:12:22 by segarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

int g_status = 0;

int ft_error(char *str, char *arg)
{
	while (*str)
		write(2, str++, 1);
	if (arg)
		while(*arg)
			write(2, arg++, 1);
	write(2, "\n", 1);
	return (1);
}`

int ft_execute(char **argv, int i, int tmp_fd, char **env)
{
    int status;

    status = 0;
    argv[i] = NULL;
    dup2(tmp_fd, 0);
    close(tmp_fd);
    status = execve(argv[0], argv, env);
    g_status = status;
    return (status);
}

int main(int argc, char **argv, char **env)
{
    int i;
    int fd[2];
    int tmp_fd;
    (void) argc;

    i = 0;
    tmp_fd = dup(0);

    while (argv[i] && argv[i + 1])
    {
        argv = &argv[i + 1];
        i = 0;
        while (argv[i] && strcmp(argv[i], ";") != 0 && strcmp(argv[i], "|") != 0)
            i++;
        if (strcmp(argv[0], "cd") == 0)
        {
            if (i != 2)
                ft_error("error: cd: bad arguments", NULL);
            else if (chdir(argv[1]) != 0)
            {
                g_status = chdir(argv[1]);
                ft_error("error: cd: cannot change directory to ", argv[1]);
                return (g_status);
            }
        }
        else if (i != 0 && (argv[i] == NULL || strcmp(argv[i], ";") == 0))
        {
            if (fork() == 0)
            {
                if (ft_execute(argv, i, tmp_fd, env) != 0)
                    return (g_status);
            }
            else
            {
                close(tmp_fd);
                while(waitpid(-1, NULL, WUNTRACED) != -1);
                tmp_fd = dup(0);
            }
        }
        else if (i != 0 && strcmp(argv[i], "|") == 0)
        {
            pipe(fd);
            if (fork() == 0)
            {
                dup2(fd[1], 1);
                close(fd[0]);
                close(fd[1]);
                if (ft_execute(argv, i, tmp_fd, env) != 0)
                    return (g_status);
            }
            else
            {
                close(fd[1]);
                close(tmp_fd);
                tmp_fd = fd[0];
            }
        }
    }
    close(tmp_fd);
    return (g_status);
}