/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tmp.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: segarcia <segarcia@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/12 15:46:43 by segarcia          #+#    #+#             */
/*   Updated: 2023/04/12 15:59:14 by segarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int g_status = 0;

void ft_error(char *str, int newline)
{
    int i = 0;

    while (str[i])
    {
        write(2, &str[i], 1);
        i++;
    }
    if (newline)
        write(2, "\n", 1);
}

int ft_execve(char **argv, char **env, int i, int fd)
{
    argv[i] = NULL;
    dup2(fd, STDIN_FILENO);
    close(fd);
    g_status = execve(argv[0], argv, env);
    return (g_status);
}

int main(int argc, char **argv, char **env)
{
    int i = 0;
    int fd[2];
    int tmp_fd = dup(STDIN_FILENO);

    (void) argc;
    (void) argv;
    (void) env;
    (void) fd;
    (void) tmp_fd;

    while (argv[i] && argv[i + 1])
    {
        argv = &argv[i + 1];
        i = 0;
        while (argv[i] && strcmp(argv[i], ";") != 0 && strcmp(argv[i], "|") != 0)
            i++;
        if (strcmp(argv[0], "cd") == 0)
        {
            if (i != 2)
            {
                g_status = 1;
                ft_error("error: cd: bad arguments", 1);
                return (g_status);
            }
            else
            {
                g_status = chdir(argv[1]);
                if (g_status != 0)
                {
                    ft_error("error: cd: cannot change directory to ", 0);
                    ft_error(argv[1], 1);
                    return (g_status);
                }
            }
        }
        else if(i != 0 && (argv[i] == NULL || strcmp(argv[i], ";") == 0))
        {
            if (fork() == 0)
            {
                g_status = ft_execve(argv, env, i, tmp_fd);
                if (g_status != 0)
                {
                    ft_error("error: cannot execute: ", 0);
                    ft_error(argv[0], 1);
                    return (g_status);
                }
            }
            else
            {
                close(tmp_fd);
                while(waitpid(-1, &g_status, 2) != -1);
                tmp_fd = dup(STDIN_FILENO);
            }
        }
        else if (i != 0 && (strcmp(argv[i], "|") == 0))
        {
            pipe(fd);
            if (fork() == 0)
            {
                dup2(fd[1], 1);
                close(fd[0]);
                close(fd[1]);
                g_status = ft_execve(argv, env, i, tmp_fd);
                if (g_status != 0)
                {
                    ft_error("error: cannot execute: ", 0);
                    ft_error(argv[0], 1);
                    return (g_status);
                }
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