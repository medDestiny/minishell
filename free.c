/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 22:46:59 by mmisskin          #+#    #+#             */
/*   Updated: 2023/05/19 18:43:57 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	clean_vec(char **vec)
{
	int	i;

	i = 0;
	while (vec[i])
	{
		free(vec[i]);
		i++;
	}
	free(vec);
}

void	clean_env_list(t_env *envp)
{
	t_env	*next;

	while (envp)
	{
		next = envp->next;
		free(envp->name);
		free(envp->value);
		free(envp);
		envp = next;
	}
}
