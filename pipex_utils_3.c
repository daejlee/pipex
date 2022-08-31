#include <stdlib.h>

static char	**awk_tquote(char *s)
{
	char	**ret;

	ret = (char **)malloc(sizeof(char *) * 3);
	if (!ret)
		return (NULL);
	ret[0] = (char *)malloc(sizeof(char) * 4);
	ret[1] = (char *)malloc(sizeof(char) * (ft_strlen(s) - 3));
	if (!ret[0] || !ret[1])
	{
		free_arr (ret);
		return (NULL);
	}
	ret[2] = NULL;
	ft_strlcpy(ret[0], "awk", 4);
	ft_strlcpy(ret[1], s + 4, ft_strlen(s) - 3);
	return (ret);
}

char	**ft_split_awk(char *s)
{
	char	**ret;

	if (ft_strchr(s, '\''))
		ret = ft_split(s, '\'');
	else if (ft_strchr(s, '\"'))
		ret = ft_split(s, '\"');
	else
		ret = awk_tquote(s);
	if (!ret)
		return (NULL);
	ret[0][3] = '\0';
	return (ret);
}
