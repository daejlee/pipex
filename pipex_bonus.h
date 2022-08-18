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

#ifndef PIPEX_BONUS_H
# define PIPEX_BONUS_H
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
}	t_fd_list;

int		free_arr(char **com);
int		err(void);
void	swap_pfd(int **pfd1, int **pfd2);
void	close_fd(int fd[2]);
void	prep_fd(int input_fd, int output_fd);
char	*ft_strjoin_modified(char const *s1, char const *s2);

#endif
