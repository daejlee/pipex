#include "./libft/libft.h"
#include <fcntl.h>
#include <sys/wait.h>
#include "./pipex.h"
// cc -g -Wall -Wextra -Werror -o pipex pipex_utils.c pipex.c -lft -L./libft
//$> ./pipex infile "grep new" "wc -w" outfile
//< infile grep new | wc -w > outfile
//valgrind --leak-check=full --trace-children=yes ./pipex infile "grep new" "wc -w" outfile
//./pipex "assets/deepthought.txt" "notexisting" "wc" "test_output.txt"
//./pipex "/dev/null" "./assets/env_var" "cat" "test_output.txt"

void	prep_fd(int input_fd, int output_fd)
{
	dup2(output_fd, 2);
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

char	**get_sh_path(char **envp)
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
			ft_strlcpy(sh_paths[0], sh_paths[0] + 5, ft_strlen(sh_paths[0]) - 4);
			return (sh_paths);
		}
		else
			i++;
	}
	return (NULL);
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


char	*get_sh_func(char **com, char **envp)
{
	char	*sh_func;
	char	**sh_paths;
	unsigned int	i;

	//envp == environment pointer
	//if (!ft_strncmp(com[0], "/bin/", 5) || !ft_strncmp(com[0], "/usr/bin/", 9))
	i = 0;
	if (!access((const char *)com[0], F_OK))
		return (com[0]);
	else
	{
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
			else
				free(sh_func);
		}
		return (NULL);
	}
}

int	exec_com(char **com, int input_fd, int output_fd, int closing_fd, char **envp)
{
	pid_t	pid;
	int		status;
	char	*sh_func;

	if (!com)
		return (1);
	close(closing_fd);
	pid = fork();
	if (pid == -1)
	{
		free_arr(com);
		return (1);
	}
	else if (!pid)
	{
		prep_fd(input_fd, output_fd);
		sh_func = get_sh_func(com, envp);
		if (!sh_func)
			exit(1);
		else
			execve((const char *)sh_func, (char * const*)com, envp);
	}
	else
	{
		free_arr(com);
		waitpid(-1, &status, WNOHANG);
		ft_printf("child pid: %i / status: %d\n", pid, status);
		return (0);
	}
	return (0);
}

int	main(int argc, char* argv[], char** envp)
{
	int		outfile_fd;
	int		infile_fd;
	int		pfd_arr[2][2];
	int		*pfd;
	int		*next_pfd;
	char	**com;
	int		i;

	infile_fd = open(argv[1], O_RDONLY);
	if (argc < 5 || infile_fd == -1)
		return(err());
	outfile_fd = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	pfd = pfd_arr[0];
	next_pfd = pfd_arr[1];
	if (pipe(pfd) == -1 || outfile_fd == -1)
	{
		close(outfile_fd);
		return(err());
	}
	i = 2;
	com = ft_split(argv[i++], ' ');
	// ./pipex "dev/null" "env_var" "cat" "test_output.txt"
	if (exec_com(com, infile_fd, pfd[1], 0, envp))
		return(err());
	close(infile_fd);
	while (i < argc - 1)
	{
		if (pipe(next_pfd) == -1)
			return(err());
		com = ft_split(argv[i++], ' ');
		if (i == argc - 1)
			dup2(outfile_fd, next_pfd[1]);
		if (exec_com(com, pfd[0], next_pfd[1], pfd[1], envp))
			return(err());
		close_fd(pfd);
		swap_pfd(&pfd, &next_pfd);
	}
	close(outfile_fd);
	close_fd(pfd);
	return (0);
}
