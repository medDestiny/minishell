/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/07 18:51:24 by mmisskin          #+#    #+#             */
/*   Updated: 2023/08/03 16:32:08 by hlaadiou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	clean_all(t_garb **gc)
{
	t_garb	*next;

	while (*gc)
	{
		next = (*gc)->next;
		free((*gc)->ptr);
		free(*gc);
		*gc = next;
	}
	*gc = NULL;
}

int	garbage_list_add(t_garb **gc, void *ptr)
{
	t_garb	*tmp;

	tmp = *gc;
	if (!*gc)
	{
		*gc = (t_garb *)malloc(sizeof(t_garb));
		tmp = *gc;
	}
	else
	{
		while (tmp && tmp->next)
			tmp = tmp->next;
		tmp->next = (t_garb *)malloc(sizeof(t_garb));
		tmp = tmp->next;
	}
	if (!tmp)
		return (1);
	tmp->ptr = ptr;
	tmp->next = NULL;
	return (0);
}

void	*ft_malloc(size_t size, t_garb **gc)
{
	void	*new;

	new = malloc(size);
	if (!new)
	{
		clean_all(gc);
		ft_putstr_fd("unexpected malloc fail occured\n", STDERR_FILENO);
	}
	else if (new && garbage_list_add(gc, new) != 0)
	{
		clean_all(gc);
		ft_putstr_fd("unexpected malloc fail occured\n", STDERR_FILENO);
		exit(EXIT_FAILURE);
	}
	return (new);
}
