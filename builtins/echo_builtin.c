/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/13 17:18:50 by mmisskin          #+#    #+#             */
/*   Updated: 2023/08/09 16:27:37 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	check_option(char **cmd, int *nl)
{
	int	i;
	int	j;

	j = 1;
	while (cmd[j])
	{
		i = 0;
		if (cmd && cmd[j][0] == '-')
			i++;
		else
			return (j);
		while (cmd[j] && cmd[j][i])
		{
			if (cmd[j][i] != 'n')
				return (j);
			i++;
		}
		*nl = 0;
		j++;
	}
	return (j);
}

void	_echo(char **cmd, int fd)
{
	int	i;
	int	nl;

	nl = 1;
	i = check_option(cmd, &nl);
	while (cmd[i])
	{
		ft_putstr_fd(cmd[i], fd);
		if (cmd[i + 1])
			ft_putstr_fd(" ", fd);
		i++;
	}
	if (nl)
		ft_putstr_fd("\n", fd);
	g_exit.status = 0;
}
