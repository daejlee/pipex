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

void	prep_fd(int input_fd, int output_fd)
{
	if (input_fd)
	{
		dup2(input_fd, 0);
		close(input_fd);
	}
	if (output_fd != 1)
	{
		dup2(output_fd, 1);
		close(output_fd);
	}
}
