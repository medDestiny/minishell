/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd_builtin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 17:02:35 by mmisskin          #+#    #+#             */
/*   Updated: 2023/06/13 16:34:03 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	check_arguments(char **cmd)
{
	int	i;

	i = 1;
	while (cmd && cmd[i])
	{
		if (cmd[i][0] == '-')
		{
			ft_putstr_fd("minishell: pwd: ", STDERR_FILENO);
			ft_putstr_fd(cmd[i], STDERR_FILENO);
			ft_putstr_fd(": invalid option\n", STDERR_FILENO);
			return (1);
		}
		i++;
	}
	return (0);
}

void	_pwd(char **cmd, int fd)
{
	char	*cwd;

	if (check_arguments(cmd) != 0)
		return ;
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		ft_putstr_fd("cd: error retrieving current directory: getcwd: \
cannot access parent directories: ", STDERR_FILENO);
		perror(NULL);
	}
	else
	{
		ft_putstr_fd(cwd, fd);
		write(fd, "\n", 1);
	}
	free(cwd);
}
