/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segarcia <segarcia@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/11 13:30:20 by segarcia          #+#    #+#             */
/*   Updated: 2023/04/11 14:11:19 by segarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define FD_READ_END 0
#define FD_WRITE_END 1

#define FD_STD_INPUT 0
#define FD_STD_OUTPUT 1
#define FD_STD_ERROR 2 

int g_status = 0;

int ft_error(char *str, char *arg)
{
    while (*str)
		write(2, str++, 1);
	if (arg)
		while(*arg)
			write(2, arg++, 1);
	write(2, "\n", 1);
    g_status = 1;
	return (g_status);
}

int ft_execute(char **argv, char **env, int i, int fd)
{
    argv[i] = NULL;
    dup2(fd, 0);
    close(fd);
    g_status = execve(argv[0], argv, env);
    return (g_status);
}

int handle_cd(char **argv, int i)
{
    if (i != 2)
        ft_error("error: cd: bad arguments", NULL);
    else
    {
        g_status = chdir(argv[1]);
        if (g_status != 0)
        {
            ft_error("error: cd: cannot change directory to ", argv[1]);
            return (g_status);
        }
    }
    return (g_status);
}

int is_continous(int i, char **argv)
{
    if (i != 0 && (argv[i] == NULL || !strcmp(argv[i], ";")))
        return (1);
    return (0);
}

int is_pipe(int i, char **argv)
{
    if (i != 0 && !strcmp(argv[i], "|"))
        return (1);
    return (0);
}

int main(int argc, char **argv, char **env)
{
    int i;
    int fd[2];
    int tmp_fd;
    (void) argc;

    i = 0;
    g_status = 0;
    tmp_fd = dup(0);
    
    while (argv[i] && argv[i + 1])
    {
        argv = &argv[i + 1];
        i = 0;
        while (argv[i] && strcmp(argv[i], ";") != 0 && strcmp(argv[i], "|") != 0)
            i++;
        if (strcmp(argv[0], "cd") == 0)
            handle_cd(argv, i);
        else if (is_continous(i, argv))
        {
            if (fork() == 0)
            {
                g_status = ft_execute(argv, env, i, tmp_fd);
                if (g_status != 0)
                    return (g_status);
            }   
            else
            {
                close(tmp_fd);
                while (waitpid(-1, &g_status, 0) != -1);
                tmp_fd = dup(0);
            }
        }
        else if (is_pipe(i, argv))
        {
            pipe(fd);
            if (fork() == 0)
            {
                dup2(fd[1], 1);
                close(fd[0]);
                close(fd[0]);
                g_status = ft_execute(argv, env, i, tmp_fd);
                if (g_status != 0)
                    return (g_status);
            }
            else
            {
                close(fd[1]);
                close(fd[tmp_fd]);
                tmp_fd = fd[0];
            }
        }
    }
    close(tmp_fd);
    return (g_status);
}
