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
#include "pipex.h"
#include <fcntl.h>
#include <stdio.h>

int	main(int argc, char *argv[], char **envp)
{
	t_pipex	*p;

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
