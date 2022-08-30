/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils_1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daejlee <daejlee@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/13 19:54:27 by daejlee           #+#    #+#             */
/*   Updated: 2022/08/13 19:54:28 by daejlee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include "./pipex.h"

void	swap_pfd(int **pfd1, int **pfd2)
{
	int	*temp;

	temp = *pfd1;
	*pfd1 = *pfd2;
	*pfd2 = temp;
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

int	check_com_for_quote(char *com)
{
	unsigned int	i;
	unsigned int	k;

	i = 0;
	while (com[i])
	{
		if (!(com[i] - '\''))
		{
			k = i + 1;
			while (com[k])
			{
				if (!(com[k] - '\''))
					return (1);
				k++;
			}
		}
		i++;
	}
	return (0);
}
