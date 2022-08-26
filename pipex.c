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

void	check_pipes(void)
{
	unsigned int	i;

	i = 0;
	while (i < 10)
	{
		if (dup(i) != -1)
			ft_printf("%i is open FD.\n", i);
		else
			ft_printf("%i is closed FD.\n", i);
		i++;
	}
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

int	after_fork(t_fd_list *p, char *argv[], int i)
{
	int		com_not_found;
	char	*sh_func;

	if (p->infile_fd == -1)
		return (0);
	p->com = ft_split(argv[i], ' ');
	if (!p->com)
		return (err_terminate(p));
	com_not_found = 0;
	sh_func = get_sh_func(p->com, p->envp, &com_not_found);
	if (com_not_found)
		perror("pipex error: command not found");
	else if (!sh_func)
	{
		free_arr(p->com);
		return (err_terminate(p));
	}
	else
		execve((const char *)sh_func, (char *const *)p->com, p->envp);
	return (0);
}

/*
A | B
B가 stdin을 닫고 A가 stdout에 쓰는 경우 A는 SIGPIPE(broken pipe)를 수신해야 합니다.
[cat /dev/random -> stdout | -> stdin head -1] THEN, head -1 close stdin, therefore cat /dev/random should receive SIGPIPE.
				next_pfd[1]		next_pfd[0] (pfd[0])

A가 stdout을 닫고 B가 stdin에서 읽으면 B는 EOF를 받아야 합니다.
A가 stdout에 아무 것도 쓰지 않고(하지만 stdout은 여전히 ​​열려 있음) B가 stdin으로 읽는 경우 B의 읽기는 차단되어야 합니다.

내가 설명하는 동작을 하려면 적절한 시간에 파이프의 fds를 닫아야 합니다.
*/

static int	exec_com(t_fd_list *p, int argc, char *argv[])
{
	pid_t	*pids;
	int		status;
	int		i;
	pid_t	pid_res;

	i = 1;
	pids = (pid_t *)malloc(sizeof(pid_t) * (argc - 2));
	pids[argc - 3] = 0;
	if (!pids)
		return (err_terminate(p));
	while (i++ < argc - 2)
	{
		if (pipe(p->next_pfd) == -1)
			return (err_terminate(p));
		if (i == 2)
			prep_fd(p->infile_fd, p->next_pfd[1], -1);
		else if (i == argc - 2)
			prep_fd(p->pfd[0], p->outfile_fd, p->pfd[1]);
		else
			prep_fd(p->pfd[0], p->next_pfd[1], p->pfd[1]);
		pids[i - 2] = fork();
		if (pids[i - 2] == -1)
			return (err_terminate(p));
		else if (!pids[i - 2])
			if (after_fork(p, argv, i))
				return (1);
		swap_pfd(&p->next_pfd, &p->pfd);
	}
	close_fd(p->next_pfd);
	i = 0;
	while (pids[i])
		waitpid(pids[i++], &status, 0);
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
