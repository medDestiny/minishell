/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environment.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 08:47:46 by mmisskin          #+#    #+#             */
/*   Updated: 2023/08/20 21:40:59 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_env	*get_env_node(t_env *env, char *name)
{
	while (env)
	{
		if (env->name && !ft_strcmp(env->name, name))
			return (env);
		env = env->next;
	}
	return (NULL);
}

void	update_env_value(t_env **env, char *name, char *new_val, int append)
{
	t_env	*node;
	char	*old_val;

	node = get_env_node(*env, name);
	if (!node)
		env_node_insert(env, name, new_val, 0);
	else
	{
		if (append == 0)
		{
			old_val = node->value;
			node->value = new_val;
		}
		else
		{
			old_val = node->value;
			if (node->value)
				node->value = ft_strjoin(node->value, new_val);
			else
				node->value = ft_strdup(new_val);
			free(new_val);
		}
		free(old_val);
		free(name);
	}
}

char	*update_shell_lvl(void)
{
	char	*old_lvl;
	int		new_lvl;

	old_lvl = getenv("SHLVL");
	if (!old_lvl)
		return (ft_itoa(1));
	new_lvl = ft_atoi(old_lvl) + 1;
	return (ft_itoa(new_lvl));
}

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

//t_env	*env_new_node(char *var)
//{
//	t_env	*env;
//
//	env = (t_env *)malloc(sizeof(t_env));
//	if (!env)
//		return (NULL);
//	if (env_init(var, &env->name, &env->value) != 0)
//		return (NULL);
//	env->hide = 0;
//	env->next = NULL;
//	return (env);
//}

//int	env_add(t_env **env, char *name, char *value, int hide)
//{
//	t_env	*tmp;
//
//	tmp = *env;
//	if (!tmp)
//	{
//		*env = (t_env *)malloc(sizeof(t_env));
//		tmp = *env;
//	}
//	else
//	{
//		while (tmp && tmp->next)
//			tmp = tmp->next;
//		tmp->next = (t_env *)malloc(sizeof(t_env));
//		tmp = tmp->next;
//	}
//	if (!tmp)
//	{
//		clean_env_list(env);
//		return (1);
//	}
//	tmp->name = name;
//	tmp->value = value;
//	tmp->hide = hide;
//	tmp->next = NULL;
//	return (0);
//}

t_env	*build_env(char *program_name)
{
	t_env	*env;
	char	*cwd;
	int		err;

	err = 0;
	env = NULL;
	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		err = env_node_insert(&env, ft_strdup("PWD"), cwd, 0);
		err = env_node_insert(&env, ft_strdup("2PWD"), ft_strdup(cwd), 1);
	}
	err = env_node_insert(&env, ft_strdup("PATH"),
			ft_strdup("/usr/gnu/bin:/usr/local/bin:/bin:/usr/bin:."), 1);
	err = env_node_insert(&env, ft_strdup("OLDPWD"), NULL, 0);
	err = env_node_insert(&env, ft_strdup("SHLVL"), ft_strdup("1"), 0);
	err = env_node_insert(&env, ft_strdup("_"), ft_strdup(program_name), 1);
	if (err != 0)
	{
		clean_env_list(&env);
		return (NULL);
	}
	return (env);
}

t_env	*build_env_list(char **env)
{
	t_env	*head;
	int		i;
	char	*name;
	char	*value;

	i = -1;
	head = NULL;
	while (env[++i])
	{
		if (env_init(env[i], &name, &value) \
			|| env_node_insert(&head, name, value, 0) != 0)
			return (clean_env_list(&head), NULL);
	}
	return (head);
}

t_env	*env_dup(char *prog_name, char **env)
{
	t_env	*envp;
	char	*cwd;

	if (!env || !*env)
		return (build_env(prog_name));
	envp = build_env_list(env);
	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		update_env_value(&envp, ft_strdup("PWD"), cwd, 0);
		env_node_insert(&envp, ft_strdup("2PWD"), ft_strdup(cwd), 1);
	}
	update_env_value(&envp, ft_strdup("SHLVL"), update_shell_lvl(), 0);
	update_env_value(&envp, ft_strdup("OLDPWD"), NULL, 0);
	return (envp);
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
