/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils_1_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daejlee <daejlee@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/23 17:49:09 by daejlee           #+#    #+#             */
/*   Updated: 2022/08/23 17:49:10 by daejlee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include "./pipex_bonus.h"

void	swap_pfd(int **pfd1, int **pfd2)
{
	int	*temp;

	temp = *pfd1;
	*pfd1 = *pfd2;
	*pfd2 = temp;
}

void	close_fd(int fd[2])
{
	close(fd[0]);
	close(fd[1]);
}

void	prep_fds(t_fd_list *p, int i, int argc)
{
	if (i == 2)
		prep(p->infile_fd, p->next_pfd[1], 0, p);
	else if (i == argc - 2)
		prep(p->pfd[0], p->outfile_fd, p->pfd[1], NULL);
	else
		prep(p->pfd[0], p->next_pfd[1], 1, p);
}

void	prep(int input_fd, int output_fd, int closing_fd, t_fd_list *p)
{
	close(closing_fd);
	if (input_fd)
	{
		dup2(input_fd, 0);
		close(input_fd);
		if (p)
			p->pfd[0] = 0;
	}
	if (output_fd != 1)
	{
		dup2(output_fd, 1);
		close(output_fd);
		if (p)
			p->next_pfd[1] = 1;
	}
}

void	execve_failed(t_fd_list *p, char *sh_func)
{
	err_terminate(p);
	free(p->pids);
	free_arr(p->com);
	if (sh_func)
		free(sh_func);
	free(p);
	exit (0);
}
