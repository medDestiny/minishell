/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dir.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <mmisskin@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/17 13:17:44 by mmisskin          #+#    #+#             */
/*   Updated: 2023/07/17 15:47:50 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include<dirent.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

int	wildcard_match(char *str, char *pattern);

typedef struct s_entry
{
	char			*name;
	struct s_entry	*next;
}	t_entry;

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

t_entry	*lst_last(t_entry *list)
{
	while (list && list->next)
		list = list->next;
	return (list);
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
		ptr = lst_last(*list);
		ptr->next = (t_entry *)malloc(sizeof(t_entry));
		ptr = ptr->next;
	}
	if (!ptr)
	{
		clean_list(list);
		return (1);
	}
	ptr->name = (char *)malloc((size + 1) * sizeof(char));
	if (!ptr->name)
	{
		clean_list(list);
		return (1);
	}
	strlcpy(ptr->name, name, size + 1);
	ptr->next = NULL;
	return (0);
}

t_entry	*dir_pattern_check(char *dir, char *pattern)
{
	t_entry			*entries;
	DIR				*dirp;
	struct dirent	*info;
	                                               
	dirp = opendir(dir);
	entries = NULL;
	if (!dirp)
		return (NULL);
	while (1)
	{
		info = readdir(dirp);
		if (info == NULL)
			break ;
		if (wildcard_match(info->d_name, pattern))
			if (entry_list_add(&entries, info->d_name, info->d_namlen) != 0)
				return (NULL);
	}
	closedir(dirp);
	return (entries);
}

int	main(int ac, char **av)
{
	t_entry	*entries;

	entries = dir_pattern_check(av[1], av[2]);
	while (entries)
	{
		printf("%s\n", entries->name);
		entries = entries->next;
	}
}
