/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 12:34:25 by mmisskin          #+#    #+#             */
/*   Updated: 2023/05/02 00:03:07 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"

char	*get_env_var(char **env, char *var)
{
	int		i;
	char	*env_var;

	i = 0;
	while (env[i] && ft_strncmp(env[i], var, ft_strlen(var)))
		i++;
	if (!env[i])
		return (NULL);
	env_var = ft_strdup(env[i] + ft_strlen(var));
	return (env_var);
}

char	**get_dir(char **env)
{
	char	**dir;
	char	cwd[1024];

	dir = (char **)malloc(3 * sizeof(char *));
	if (!dir)
		return (NULL);
	dir[2] = NULL;
	dir[0] = get_env_var(env, "HOME=");
	dir[1] = ft_strdup(getcwd(cwd, 1024));
	return (dir);
}

char	*prompt(char **env)
{
	char	**dir;
	char	*cwd;
	char	*res;

	dir = get_dir(env);
	if (!ft_strncmp(dir[0], dir[1], ft_strlen(dir[0])))
	{
		cwd = ft_strjoin("~", dir[1] + ft_strlen(dir[0]));
		res = ft_strjoin(cwd, "$ ");
		free(cwd);
		clean_vec(dir);
		return (res);
	}
	res = ft_strjoin(dir[1], "$ ");
	clean_vec(dir);
	return (res);
}

void	builtin_cd(char *cmdline, char **env)
{
	char	*cd;

	if (ft_strncmp(cmdline, "cd", 2))
	{
		printf("cd bo7dha li 5dama db hh\n");
		return ;
	}
	if (ft_strlen(cmdline) == 2)
		cd = get_env_var(env, "HOME=");
	else
		cd = cmdline + 3;
	if (chdir(cd) != 0)
		perror(NULL);
}

int	main(int ac, char **av, char **env)
{
	char	*line;
	char	*shell;

	line = NULL;
	(void)av;
	if (ac != 1 || !env)
		return (1);
	while (1)
	{
		shell = prompt(env);
		line = readline((const char *)shell);
		free(shell);
		if (!line)
			break ;
		builtin_cd(line, env);
		free(line);
	}
}
