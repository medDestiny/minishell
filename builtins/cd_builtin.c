/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_builtin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 18:28:51 by mmisskin          #+#    #+#             */
/*   Updated: 2023/06/10 21:08:58 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	cd(char **cmd, t_env *env)
{
	char	*dir;

	if (!cmd[1])
	{
		dir = get_env_value(env, "HOME");
		if (!dir)
		{
			ft_putstr_fd("minishell: cd: HOME not set\n", STDERR_FILENO);
			return ;
		}
	}
	else if (!ft_strcmp(cmd[1], "-"))
	{
		dir = get_env_value(env, "OLDPWD");
		if (!dir)
		{
			ft_putstr_fd("minishell: cd: OLDPWD not set\n", STDERR_FILENO);
			return ;
		}
	}
	else
		dir = cmd[1];
	if (chdir(dir) != 0)
	{
		if (ft_strcmp(cmd[1], ".."))
		{
			ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
			ft_putstr_fd(dir, STDERR_FILENO);
			ft_putstr_fd(": ", STDERR_FILENO);
		}
		else
		{
			ft_putstr_fd("cd: error retrieving current directory: getcwd: cannot access parent directories: ", STDERR_FILENO);
		}
		perror(NULL);
	}
	else
		printf("hh\n");
}
