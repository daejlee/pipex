/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daejlee <daejlee@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/13 19:54:38 by daejlee           #+#    #+#             */
/*   Updated: 2022/08/13 19:54:38 by daejlee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "libft.h"
#include "pipex.h"
#include <stdio.h>
#include <sys/wait.h>

t_pipex	*init_p(void)
{
	t_pipex	*ret;

	ret = (t_pipex *)malloc(sizeof(t_pipex));
	if (!ret)
		return (NULL);
	ret->pfd = NULL;
	ret->next_pfd = NULL;
	ret->envp = NULL;
	ret->com = NULL;
	return (ret);
}

int	free_arr(char **com)
{
	unsigned int	i;

	i = 0;
	while (com[i])
		free(com[i++]);
	free(com);
	return (0);
}

int	err_terminate(t_pipex *p)
{
	perror("pipex error");
	if (p->infile_fd != -1)
		close(p->infile_fd);
	if (p->outfile_fd != -1)
		close(p->outfile_fd);
	if (p->pfd)
	{
		close(p->pfd[0]);
		close(p->pfd[1]);
	}
	if (p->next_pfd)
	{
		close(p->next_pfd[0]);
		close(p->next_pfd[1]);
	}
	close(0);
	close(1);
	return (1);
}

void	execve_failed(t_pipex *p, char *sh_func)
{
	err_terminate(p);
	free(p->pids);
	free_arr(p->com);
	if (sh_func)
		free(sh_func);
	free(p);
	exit (0);
}

int	wait_for_children(t_pipex *p, pid_t *pids)
{
	int	i;
	int	status;

	status = 0;
	close(p->pfd[0]);
	close(p->pfd[1]);
	close(p->next_pfd[0]);
	close(p->next_pfd[1]);
	close(0);
	close(1);
	free(p);
	i = 0;
	while (pids[i])
		waitpid(pids[i++], &status, 0);
	free(pids);
	return (status);
}
