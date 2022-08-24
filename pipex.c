/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daejlee <daejlee@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/13 19:53:55 by daejlee           #+#    #+#             */
/*   Updated: 2022/08/13 19:53:56 by daejlee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./libft_garage/libft/libft.h"
#include "./libft_garage/ft_printf/ft_printf.h"
#include "./pipex.h"
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>

t_fd_list	*init_p(void)
{
	t_fd_list	*ret;

	ret = (t_fd_list *)malloc(sizeof(t_fd_list));
	if (!ret)
		return (NULL);
	ret->pfd = NULL;
	ret->next_pfd = NULL;
	ret->envp = NULL;
	ret->com = NULL;
	return (ret);
}

static char	**get_sh_path(char **envp)
{
	unsigned int	i;
	char			**sh_paths;

	i = 0;
	while (envp[i])
	{
		if (!ft_strncmp("PATH", envp[i], 4))
		{
			sh_paths = ft_split(envp[i], ':');
			if (!sh_paths)
				return (NULL);
			ft_strlcpy(sh_paths[0], sh_paths[0] + 5,
				ft_strlen(sh_paths[0]) - 4);
			return (sh_paths);
		}
		else
			i++;
	}
	return (NULL);
}

static char	*get_sh_func(char **com, char **envp, int *flag)
{
	char			*sh_func;
	char			**sh_paths;
	unsigned int	i;

	i = 0;
	if (!access((const char *)com[0], X_OK))
		return (com[0]);
	sh_paths = get_sh_path(envp);
	if (!sh_paths)
		return (NULL);
	while (sh_paths[i])
	{
		sh_func = ft_strjoin_modified(sh_paths[i++], com[0]);
		if (!access(sh_func, X_OK) || !sh_func)
		{
			free_arr(sh_paths);
			return (sh_func);
		}
		free(sh_func);
	}
	*flag = 1;
	ft_printf("pipex error: command not found: %s\n", com[0]);
	free_arr(sh_paths);
	return (NULL);
}

int	after_fork(t_fd_list *p, int input_fd, char *argv[], int i)
{
	int		com_not_found;
	char	*sh_func;

	p->com = ft_split(argv[i], ' ');
	if (!p->com)
		return (err_terminate(p));
	com_not_found = 0;
	sh_func = get_sh_func(p->com, p->envp, &com_not_found);
	if (com_not_found || dup(input_fd) == -1)
		return (0);
	else if (!sh_func)
	{
		free_arr(p->com);
		return (err_terminate(p));
	}
	execve((const char *)sh_func, (char *const *)p->com, p->envp);
	return (0);
}

static int	exec_com(t_fd_list *p, int argc, char *argv[])
{
	pid_t	pid;
	int		status;
	int		i;

	i = 1;
	while (i++ < argc - 2)
	{
		if (pipe(p->next_pfd) == -1)
			return (err_terminate(p));
		if (i == 2)
			prep_fd(p->infile_fd, p->next_pfd[1], p->next_pfd[0]);
		else if (i == argc - 2)
			dup2(p->outfile_fd, p->next_pfd[1]);
		else
			prep_fd(p->pfd[0], p->next_pfd[1], p->next_pfd[0]);
		pid = fork();
		if (pid == -1)
			return (err_terminate(p));
		else if (!pid)
			after_fork(p, p->pfd[0], argv, i);
		close_fd(p->pfd);
		swap_pfd(&p->next_pfd, &p->pfd);
	}
	waitpid(-1, &status, 0);
	return (0);
}

int	main(int argc, char *argv[], char **envp)
{
	t_fd_list	*p;

	if (argc < 5)
		return (1);
	p = init_p();
	p->envp = envp;
	p->infile_fd = open(argv[1], O_RDONLY);
	if (p->infile_fd == -1)
		perror("pipex error");
	p->outfile_fd = open(argv[argc - 1], O_WRONLY | O_TRUNC | O_CREAT, 0644);
	if (p->outfile_fd == -1)
		return (err_terminate(p));
	p->pfd = p->pfd_arr[0];
	p->next_pfd = p->pfd_arr[1];
	return (exec_com(p, argc, argv));
}
