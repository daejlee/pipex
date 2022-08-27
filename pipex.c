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

static char	*get_sh_func(char **com, char **envp)
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
	sh_func = ft_strjoin_modified(sh_paths[0], com[0]);
	free_arr(sh_paths);
	return (sh_func);
}

static void	exec_sh(t_fd_list *p, char *argv[], int i)
{
	char	*sh_func;

	if (p->infile_fd == -1 && i == 2)
		execve_failed(p, NULL);
	close(p->next_pfd[0]);
	p->com = ft_split(argv[i], ' ');
	if (!p->com)
		exit (err_terminate(p));
	sh_func = get_sh_func(p->com, p->envp);
	if (!sh_func)
	{
		free_arr(p->com);
		exit (err_terminate(p));
	}
	execve((const char *)sh_func, (char *const *)p->com, p->envp);
	execve_failed(p, sh_func);
}

static int	exec_fork(t_fd_list *p, int argc, char *argv[])
{
	int		i;

	i = 1;
	p->pids = (pid_t *)malloc(sizeof(pid_t) * (argc - 2));
	if (!p->pids)
		return (err_terminate(p));
	p->pids[argc - 3] = 0;
	while (i++ < argc - 2)
	{
		if (pipe(p->next_pfd) == -1)
			return (err_terminate(p));
		if (p->infile_fd == -1 && i == 2)
			close(p->next_pfd[1]);
		else
		{
			prep_fds(p, i, argc);
			p->pids[i - 2] = fork();
			if (p->pids[i - 2] == -1)
				return (err_terminate(p));
			else if (!p->pids[i - 2])
				exec_sh(p, argv, i);
		}
		swap_pfd(&p->next_pfd, &p->pfd);
	}
	return (wait_for_children(p, p->pids));
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
	return (exec_fork(p, argc, argv));
}
