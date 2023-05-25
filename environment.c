/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 08:47:46 by mmisskin          #+#    #+#             */
/*   Updated: 2023/05/24 21:00:07 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	env_init(char *var, char **name, char **value)
{
	int	len;
	int	i;

	len = 0;
	i = -1;
	while (var[len] && var[len] != '=')
		len++;
	*name = (char *)malloc((len + 1) * sizeof(char));
	if (!*name)
		return (1);
	(*name)[len] = '\0';
	while (++i < len)
		(*name)[i] = var[i];
	*value = ft_strdup(getenv(*name));
	if (!value)
	{
		free(name);
		return (1);
	}
	return (0);
}

t_env	*env_new_node(char *var)
{
	t_env	*env;

	env = (t_env *)malloc(sizeof(t_env));
	if (!env)
		return (NULL);
	if (env_init(var, &env->name, &env->value) != 0)
		return (NULL);
	env->next = NULL;
	return (env);
}

t_env	*env_dup(char **env)
{
	t_env	*envp;
	t_env	*head;
	int		i;

	envp = NULL;
	if (!env)
		return (NULL);
	i = -1;
	head = NULL;
	while (env[++i])
	{
		if (!envp)
		{
			envp = env_new_node(env[i]);
			head = envp;
		}
		else
		{
			envp->next = env_new_node(env[i]);
			envp = envp->next;
		}
		if (!envp)
			return (NULL);
	}
	return (head);
}

char	*get_env_value(t_env *env, char *name)
{
	while (env)
	{
		if (!ft_strcmp(name, env->name))
			return (env->value);
		env = env->next;
	}
	return (NULL);
}
