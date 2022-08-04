#ifndef PIPEX_H
# define PIPEX_H
void	free_arr(void **com);
int		err();
void	swap_pfd(int **pfd1, int **pfd2);
void	close_fd(int fd[2]);

#endif