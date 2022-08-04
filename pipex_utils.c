#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void	free_arr(void **com)
{
	unsigned int	i;

	i = 0;
	while (com[i])
		free(com[i++]);
	free(com);
}

int		err()
{
	perror("ERROR!");
	return (1);
}

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
