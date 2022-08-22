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
	if (!access((const char *)com[0], F_OK))
		return (com[0]);
	sh_paths = get_sh_path(envp);
	if (!sh_paths)
		return (NULL);
	while (sh_paths[i])
	{
		sh_func = ft_strjoin_modified(sh_paths[i++], com[0]);
		if (!sh_func || !access(sh_func, F_OK))
		{
			free_arr(sh_paths);
			if (sh_func)
				return (sh_func);
			return (NULL);
		}
		free(sh_func);
	}
	free_arr(sh_paths);
	return (NULL);
}

static int	exec_com(t_fd_list p, int input_fd, int output_fd, int closing_fd)
{
	pid_t	pid;
	int		status;
	char	*sh_func;

	close(closing_fd);
	pid = fork();
	if (pid == -1)
	{
		free_arr(p.com);
		return (err());
	}
	else if (!pid)
	{
		sh_func = get_sh_func(p.com, p.envp);
		if (!sh_func)
		{
			ft_printf("pipex: command not found: %s\n", p.com[0]);
			exit(1);
		}
		prep_fd(input_fd, output_fd);
		execve((const char *)sh_func, (char *const *)p.com, p.envp);
	}
	else
		waitpid(-1, &status, WNOHANG);
	return (free_arr(p.com));
}

static int	execute(t_fd_list p, int argc, char *argv[])
{
	int	i;

	i = 2;
	p.com = ft_split(argv[i++], ' ');
	if (!p.com)
		return (err());
	if (exec_com(p, p.infile_fd, p.pfd[1], 0))
		return (1);
	close(p.infile_fd);
	while (i < argc - 1)
	{
		if (pipe(p.next_pfd) == -1)
			return (err());
		p.com = ft_split(argv[i++], ' ');
		if (!p.com)
			return (err());
		if (i == argc - 1)
			dup2(p.outfile_fd, p.next_pfd[1]);
		if (exec_com(p, p.pfd[0], p.next_pfd[1], p.pfd[1]))
			return (1);
		close_fd(p.pfd);
		swap_pfd(&p.pfd, &p.next_pfd);
	}
	close(p.outfile_fd);
	return (0);
}

int	main(int argc, char *argv[], char **envp)
{
	t_fd_list	p;

	if (argc < 5)
		return (1);
	p.envp = envp;
	p.infile_fd = open(argv[1], O_RDONLY);
	if (p.infile_fd == -1)
		return (err());
	p.outfile_fd = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (p.outfile_fd == -1)
	{
		close(p.infile_fd);
		return (err());
	}
	p.pfd = p.pfd_arr[0];
	p.next_pfd = p.pfd_arr[1];
	if (pipe(p.pfd) == -1)
		return (err());
	return (execute(p, argc, argv));
}
