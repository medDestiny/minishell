/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dir.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <mmisskin@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/17 13:17:44 by mmisskin          #+#    #+#             */
/*   Updated: 2023/08/09 23:41:20 by hlaadiou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	clean_list(t_entry **list)
{
	t_entry	*next;

	while (*list)
	{
		next = (*list)->next;
		free((*list)->name);
		free(*list);
		*list = next;
	}
}

t_entry	*_lst_last(t_entry *list)
{
	while (list && list->next)
		list = list->next;
	return (list);
}

int	fill_entry_node(t_entry **ptr, char *name, int size)
{
	(*ptr)->name = (char *)malloc((size + 1) * sizeof(char));
	if (!(*ptr)->name)
		return (1);
	ft_strlcpy((*ptr)->name, name, size + 1);
	(*ptr)->next = NULL;
	return (0);
}

int	entry_list_add(t_entry **list, char *name, int size)
{
	t_entry	*ptr;

	if (!*list)
	{
		*list = (t_entry *)malloc(sizeof(t_entry));
		ptr = *list;
	}
	else
	{
		ptr = _lst_last(*list);
		ptr->next = (t_entry *)malloc(sizeof(t_entry));
		ptr = ptr->next;
	}
	if (!ptr || fill_entry_node(&ptr, name, size) != 0)
	{
		clean_list(list);
		return (1);
	}
	return (0);
}

t_entry	*dir_pattern_check(char *dir, char *pattern, int *flags)
{
	t_entry			*entries;
	DIR				*dirp;
	struct dirent	*info;

	if (!pattern)
		return (NULL);
	if (!dir)
		dirp = opendir(".");
	else
		dirp = opendir(dir);
	entries = NULL;
	if (!dirp)
		return (NULL);
	while (1)
	{
		info = readdir(dirp);
		if (info == NULL)
			break ;
		if (wildcard_match(info->d_name, pattern, flags))
			if (entry_list_add(&entries, info->d_name, info->d_namlen) != 0)
				return (NULL);
	}
	closedir(dirp);
	return (entries);
}

//int	main(int ac, char **av)
//{
//	t_entry	*entries;
//	int		f[] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
//	char	d[] = "parser";
//	char	p[] = "***************?";
//
//	entries = dir_pattern_check(av[1], av[2], f);
//	while (entries)
//	{
//		printf("%s\n", entries->name);
//		entries = entries->next;
//	}
//}
