/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_builtin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 18:28:51 by mmisskin          #+#    #+#             */
/*   Updated: 2023/06/12 16:00:54 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	cd_home(t_env *env, char **home)
{
	*home = get_env_value(env, "HOME");
	if (!*home)
	{
		ft_putstr_fd("minishell: cd: HOME not set\n", STDERR_FILENO);
		return (1);
	}
	return (0);
}

int	cd_old_pwd(t_env *env, char **old_pwd)
{
	*old_pwd = get_env_value(env, "OLDPWD");
	if (!*old_pwd)
	{
		ft_putstr_fd("minishell: cd: OLDPWD not set\n", STDERR_FILENO);
		return (1);
	}
	return (0);
}

void	change_dir(t_env **env, char *dir, char **cmd, int fd)
{
	char	*cwd;

	if (chdir(dir) != 0)
	{
		ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
		ft_putstr_fd(dir, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		perror(NULL);
	}
	else
	{
		cwd = getcwd(NULL, 0);
		if (!cwd && !ft_strcmp(cmd[1], ".."))
		{
			update_env_value(env, "PWD",
				ft_strjoin(get_env_value(*env, "PWD"), "/.."), 0);
			ft_putstr_fd("cd: error retrieving current directory: getcwd: \
cannot access parent directories: ", STDERR_FILENO);
			perror(NULL);
		}
		else if (cmd[1] && !ft_strcmp(cmd[1], "-"))
			_pwd(NULL, fd);
		if (cwd)
			update_env_value(env, "PWD", cwd, 0);
	}
}

void	_cd(char **cmd, t_env *env, int fd)
{
	char	*dir;
	char	*pwd;

	if (!cmd[1])
	{
		if (cd_home(env, &dir) != 0)
			return ;
	}
	else if (!ft_strcmp(cmd[1], "-"))
	{
		if (cd_old_pwd(env, &dir) != 0)
			return ;
	}
	else
		dir = cmd[1];
	pwd = ft_strdup(get_env_value(env, "PWD"));
	change_dir(&env, dir, cmd, fd);
	update_env_value(&env, "OLDPWD", pwd, 0);
}
