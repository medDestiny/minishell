/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   insert_sort.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <mmisskin@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/20 15:27:52 by mmisskin          #+#    #+#             */
/*   Updated: 2023/08/20 21:41:26 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_env	*new_env_node(char *name, char *value, int hide)
{
	t_env	*var;

	var = (t_env *)malloc(sizeof(t_env));
	if (!var)
		return (NULL);
	var->name = name;
	var->value = value;
	var->hide = hide;
	var->next = NULL;
	return (var);
}

void	env_insert(t_env **env, t_env *node, char *name)
{
	t_env	*curr;
	t_env	*next;

	curr = *env;
	next = NULL;
	while (curr)
	{
		next = curr->next;
		if (next && !next->hide && ft_strcmp(next->name, name) > 0)
			break ;
		if (!curr->next)
			break ;
		curr = curr->next;
	}
	if (!*env)
		*env = node;
	else if (*env && !next)
		curr->next = node;
	else
	{
		curr->next = node;
		node->next = next;
	}
}

int	env_node_insert(t_env **env, char *name, char *val, int hide)
{
	t_env	*node;

	node = new_env_node(name, val, hide);
	if (!node)
		return (1);
	if (*env && ft_strcmp((*env)->name, name) > 0)
	{
		node->next = *env;
		*env = node;
		return (0);
	}
	env_insert(env, node, name);
	return (0);
}

t_entry	*new_entry_node(char *name)
{
	t_entry	*node;

	node = (t_entry *)malloc(sizeof(t_entry));
	if (!node)
		return (NULL);
	node->name = ft_strdup(name);
	if (!node->name)
		return (NULL);
	node->next = NULL;
	return (node);
}

void	entry_insert(t_entry **entries, t_entry *node, char *name)
{
	t_entry	*curr;
	t_entry	*next;

	curr = *entries;
	next = NULL;
	while (curr)
	{
		next = curr->next;
		if (next && ft_strcmp(next->name, name) > 0)
			break ;
		if (!curr->next)
			break ;
		curr = curr->next;
	}
	if (!*entries)
		*entries = node;
	else if (*entries && !next)
		curr->next = node;
	else
	{
		curr->next = node;
		node->next = next;
	}
}

int	entry_node_insert(t_entry **list, char *name)
{
	t_entry	*node;

	node = new_entry_node(name);
	if (!node)
		return (1);
	if (*list && ft_strcmp((*list)->name, name) > 0)
	{
		node->next = *list;
		*list = node;
		return (0);
	}
	entry_insert(list, node, name);
	return (0);
}
