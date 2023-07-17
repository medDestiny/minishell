/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <mmisskin@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/16 13:44:32 by mmisskin          #+#    #+#             */
/*   Updated: 2023/07/16 22:04:48 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

typedef struct s_data
{
	int	i;
	int	j;
	int	str_i;
	int	pat_i;
}	t_data;

int	check_pattern(char *str, char *pattern, t_data *data)
{
	while (str[data->i])
	{
		if (pattern[data->j] && (pattern[data->j] == str[data->i] || pattern[data->j] == '?'))
		{
			data->i++;
			data->j++;
		}
		else if (pattern[data->j] && pattern[data->j] == '*')
		{
			data->pat_i = data->j;
			data->str_i = data->i;
			data->j++;
		}
		else if (data->pat_i != -1)
		{
			data->i = data->str_i + 1;
			data->j = data->pat_i + 1;
			data->str_i++;
		}
		else
			return (0);
	}
	return (1);
}

int	wildcard_match(char *str, char *pattern)
{
	t_data	data;

	data.j = 0;
	data.i = 0;
	data.str_i = 0;
	data.pat_i = 0;
	if (!str || !str[0])
		return (0);
	if (!check_pattern(str, pattern, &data))
		return (0);
	if (str[0] == '.' && pattern[0] != '.')
		return (0);
	while (pattern[data.j] && pattern[data.j] == '*')
		data.j++;
	if (!pattern[data.j])
		return (1);
	return (0);
}

#include<stdio.h>

int	main(int ac, char **av)
{
	if (wildcard_match(av[1], av[2]))
		printf("YES\n");
	else
		printf("NO\n");
}
