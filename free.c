/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 22:46:59 by mmisskin          #+#    #+#             */
/*   Updated: 2023/08/08 03:34:32 by hlaadiou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	clean_vec(char **vec)
{
	int	i;

	i = 0;
	while (vec && vec[i])
	{
		free(vec[i]);
		i++;
	}
	free(vec);
}

void	clean_intvec(int **intvec)
{
	int	i;

	i = 0;
	while (intvec && intvec[i])
	{
		free(intvec[i]);
		i++;
	}
	free(intvec);
}

void	clean_env_list(t_env **envp)
{
	t_env	*next;

	while (*envp)
	{
		next = (*envp)->next;
		free((*envp)->name);
		free((*envp)->value);
		free(*envp);
		*envp = next;
	}
}
