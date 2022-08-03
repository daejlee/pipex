#include "./libft/ft_printf.h" //헤더가 없으면 gnl리턴이 안되더라..
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
// cc -g -o pipex pipex.c -lft -L./libft
// ./pipex infile
//$> ./pipex infile "grep new" "wc -w" outfile
//< infile grep new | wc -w > outfile


	/*ex) ./pipex infile "grep new" "wc -w" outfile
	->	ret[0] = infile
		ret[1] = grep
		ret[2] = new
		ret[3] = wc
		ret[4] = -w
		ret[5] = outfile
	*/

int	main(int argc, char* argv[])
{
	char	*com[] = {"echo", "infile", (char *) 0};
	execve("/bin/echo", com, NULL);

	return (0);
}