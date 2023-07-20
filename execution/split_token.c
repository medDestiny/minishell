/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_token.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaadiou <hlaadiou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/17 13:51:00 by hlaadiou          #+#    #+#             */
/*   Updated: 2023/07/20 08:21:32 by hlaadiou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	get_wordsize(char *lexeme, int i)
{
	int	wd_len;

	wd_len = 0;
	while (lexeme[i] && lexeme[i] != '$')
	{
		i++;
		wd_len++;
	}
	return (wd_len);
}

//$12_2X$
int	get_idsize(char *lexeme, int i)
{
	int	id_len;

	id_len = 1;
	while (lexeme[++i])
	{
		if (lexeme[i] == '$' || lexeme[i] == '?'\
			|| ft_isdigit(lexeme[i]) == 1)
		{
			id_len++;
			break ;
		}
		else
		{
			while (ft_isalpha(lexeme[i]) == 1 || lexeme[i] == '_'\
					|| (ft_isdigit(lexeme[i]) == 1 && id_len > 1))
			{
				i++;
				id_len++;
			}
			break ;
		}
	}
	return (id_len);
}

int	get_subtkn(t_token **lst, char *lexeme, int size, int i)
{
	char	*sub;

	sub = ft_substr(lexeme, i, size);
	token_list_add(lst, WORD, sub, size);
	free (sub);
	return (size - 1);
}

//Subtokens of : |hii$hi?hello$$$123_cv-ls$PATH$hh$$hello!$?|
// |hii|->|$hi|->|?hello|->|$$|->|$1|->|23_cv-ls|->|$PATH|->|$hh|->|$$|->|hello!|->|$?|
t_token	*tkn_split(char *lexeme)
{
	t_token	*subtkn_lst;
	char	*sub;
	int		i;
	int		j;

	i = 0;
	j = 0;
	subtkn_lst = NULL;
	sub = NULL;
	if (!lexeme)
		return (NULL);
	while (lexeme[i])
	{
		if (lexeme[i] == '$')
			i += get_subtkn(&subtkn_lst, lexeme, \
					get_idsize(lexeme, i), i);
		else
			i += get_subtkn(&subtkn_lst, lexeme, \
					get_wordsize(lexeme, i), i);
		i++;
	}
	return (subtkn_lst);
}
