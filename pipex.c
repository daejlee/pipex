#include "./libft/libft.h"
#include <fcntl.h>
#include <sys/wait.h>
#include "./pipex.h"
// cc -g -o pipex pipex_utils.c pipex.c -lft -L./libft
//$> ./pipex infile "grep new" "wc -w" outfile
//< infile grep new | wc -w > outfile
//valgrind --leak-check=full --trace-children=yes ./pipex infile "grep new" "wc -w" outfile
//./pipex "assets/deepthought.txt" "notexisting" "wc" "test_output.txt"

char	*get_sh_func(char **com)
{
	char	*sh_func;

	if (!ft_strncmp(com[0], "/bin/", 5) || !ft_strncmp(com[0], "/usr/bin/", 9))
		sh_func = com[0];
	else
	{
		sh_func = (char *)malloc(ft_strlen("/bin/") + ft_strlen(com[0]) + 1);
		if (!sh_func)
			return (NULL);
		ft_strlcpy(sh_func, "/bin/", 6);
		ft_strlcpy(sh_func + 5, com[0], ft_strlen(com[0]) + 1);
	}
	return (sh_func);
}

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
	else if (!pid)  //child
	{
		prep_fd(input_fd, output_fd);
		sh_func = get_sh_func(com);
		if (!sh_func || access((const char *)sh_func, F_OK))
			exit(1);
		else
			execve((const char *)sh_func, (char * const*)com, envp);
	}
	else
	{
		free_arr(com);
		wait(&status);
		ft_printf("child pid: %i / status: %d / wstatus: %d\n", pid, status, WIFEXITED(status));
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
	if (exec_com(com, infile_fd, pfd[1]/*outfile_fd*/, 0, envp))
		return(err());
	close(infile_fd);
	//존재하지 않는 명령어 실행 후 파이프 연결의 경우 일치하지 않음
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
