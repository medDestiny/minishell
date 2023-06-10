/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd_builtin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 17:02:35 by mmisskin          #+#    #+#             */
/*   Updated: 2023/06/10 18:29:57 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	pwd(char **cmd, int fd)
{
	char	*cwd;
	int		i;

	i = 0;
	while (cmd && cmd[i])
	{
		if (cmd[i][0] == '-')
		{
			ft_putstr_fd("minishell: pwd: ", STDERR_FILENO);
			ft_putstr_fd(cmd[i], STDERR_FILENO);
			ft_putstr_fd(": invalid option\n", STDERR_FILENO);
			return ;
		}
		i++;
	}
	cwd = getcwd(NULL, 0);
	ft_putstr_fd(cwd, fd);
	write(fd, "\n", 1);
	free(cwd);
}
