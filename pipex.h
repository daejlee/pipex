/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daejlee <daejlee@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/13 19:54:15 by daejlee           #+#    #+#             */
/*   Updated: 2022/08/13 19:54:16 by daejlee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H
# include <unistd.h>

typedef struct fd_list
{
	int		outfile_fd;
	int		infile_fd;
	int		pfd_arr[2][2];
	int		*pfd;
	int		*next_pfd;
	char	**envp;
	char	**com;
	pid_t	*pids;
}	t_fd_list;

void		swap_pfd(int **pfd1, int **pfd2);
void		close_fd(int fd[2]);
void		prep_fds(t_fd_list *p, int i, int argc);
void		prep(int input_fd, int output_fd, int closing_fd, t_fd_list *p);
void		execve_failed(t_fd_list *p, char *sh_func);
t_fd_list	*init_p(void);
int			free_arr(char **com);
int			err_terminate(t_fd_list *p);
char		*ft_strjoin_modified(char const *s1, char const *s2);
int			wait_for_children(t_fd_list *p, pid_t *pids);

#endif
