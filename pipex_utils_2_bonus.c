/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils_2_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daejlee <daejlee@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/23 17:49:17 by daejlee           #+#    #+#             */
/*   Updated: 2022/08/23 17:49:18 by daejlee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./libft_garage/libft/libft.h"
#include "./pipex_bonus.h"
#include <stdio.h>
#include <sys/wait.h>

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

int	free_arr(char **com)
{
	unsigned int	i;

	i = 0;
	while (com[i])
		free(com[i++]);
	free(com);
	return (0);
}

int	err_terminate(t_fd_list *p)
{
	perror("pipex error");
	if (p->infile_fd != -1)
		close(p->infile_fd);
	if (p->outfile_fd != -1)
		close(p->outfile_fd);
	if (p->pfd)
		close_fd(p->pfd);
	if (p->next_pfd)
		close_fd(p->next_pfd);
	close(0);
	close(1);
	return (1);
}

char	*ft_strjoin_modified(char const *s1, char const *s2)
{
	int				i;
	unsigned int	s1_len;
	char			*res;

	if (!s1 || !s2)
		return (0);
	s1_len = ft_strlen(s1);
	res = (char *)malloc(sizeof(char) * (s1_len + ft_strlen(s2) + 2));
	if (!res)
		return (0);
	i = 0;
	while (s1[i])
	{
		res[i] = s1[i];
		i++;
	}
	res[i] = '/';
	i = 0;
	while (s2[i])
	{
		res[s1_len + 1 + i] = s2[i];
		i++;
	}
	res[s1_len + 1 + i] = '\0';
	return (res);
}

int	wait_for_children(t_fd_list *p, pid_t *pids)
{
	int	i;
	int	status;

	status = 0;
	close_fd(p->pfd);
	close_fd(p->next_pfd);
	close(0);
	close(1);
	free(p);
	i = 0;
	while (pids[i])
		waitpid(pids[i++], &status, 0);
	free(pids);
	return (status);
}
