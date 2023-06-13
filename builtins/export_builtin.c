/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_builtin.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/12 13:54:02 by mmisskin          #+#    #+#             */
/*   Updated: 2023/06/13 16:20:42 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	print_env(t_env *env, int fd)
{
	while (env)
	{
		ft_putstr_fd("declare -x ", fd);
		ft_putstr_fd(env->name, fd);
		if (env->value)
		{
			ft_putstr_fd("=\"", fd);
			ft_putstr_fd(env->value, fd);
			ft_putstr_fd("\"", fd);
		}
		ft_putstr_fd("\n", fd);
		env = env->next;
	}
}

static void	print_invalid_id(char *id)
{
	ft_putstr_fd("minishell: export: `", STDERR_FILENO);
	ft_putstr_fd(id, STDERR_FILENO);
	ft_putstr_fd("\': not a valid identifier\n", STDERR_FILENO);
}

char	*get_value(t_env *env, char *id, char *name)
{
	int		size;
	t_env	*old;

	size = 0;
	if (!name)
		return (NULL);
	while (id[size] && id[size] != '=')
		size++;
	old = get_env_node(env, name);
	if (!id[size] && old && old->value)
		return (ft_strdup(old->value));
	if (!id[size])
		return (NULL);
	return (ft_strdup(id + size + 1));
}

int	check_name(char *arg, int *append)
{
	int	i;
	int	size;

	i = 1;
	size = 1;
	while (arg[i] && arg[i] != '=')
	{
		if (arg[i] == '+' && arg[i + 1] == '=')
			*append = 1;
		else if (!ft_isalnum(arg[i]) && arg[i] != '_')
			return (-1);
		if (arg[i] != '+')
			size++;
		i++;
	}
	return (size);
}

static char	*check_identifier(char *arg, int *append)
{
	int		i;
	int		size;
	char	*name;

	i = 1;
	size = 1;
	if (!ft_isalpha(arg[0]) && arg[0] != '_')
		size = -1;
	else
		size = check_name(arg, append);
	if (size < 0)
	{
		print_invalid_id(arg);
		return (NULL);
	}
	name = (char *)malloc((size + 1) * sizeof(char));
	if (!name)
		return (NULL);
	ft_strlcpy(name, arg, size + 1);
	return (name);
}

void	_export(char **cmd, t_env **env, int fd)
{
	int		i;
	int		app;
	char	*name;
	char	*value;

	i = 1;
	if (!cmd[1])
		print_env(*env, fd);
	else
	{
		while (cmd[i])
		{
			app = 0;
			name = check_identifier(cmd[i], &app);
			value = get_value(*env, cmd[i], name);
			if (value && name)
				update_env_value(env, ft_strdup(name), ft_strdup(value), app);
			else if (name)
				update_env_value(env, ft_strdup(name), value, app);
			free(value);
			free(name);
			i++;
		}
	}
}
