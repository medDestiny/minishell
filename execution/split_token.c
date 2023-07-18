/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_token.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaadiou <hlaadiou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/17 13:51:00 by hlaadiou          #+#    #+#             */
/*   Updated: 2023/07/18 20:43:24 by hlaadiou         ###   ########.fr       */
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

//Subtokens of : |hii$hi?hello$$$123_cv-ls$PATH$hh$$hello!$?|
// |hii|->|$hi|->|?hello|->|$$|->|$1|->|23_cv-ls|->|$PATH|->|$hh|->|$$|->|hello!|->|$?|
t_token	*tkn_split(char *lexeme)
{
	t_token	*subtkn_lst;
	int		size;
	int		i;
	int		j;

	i = 0;
	j = 0;
	subtkn_lst = NULL;
	if (!lexeme)
		return (NULL);
	while (lexeme[i])
	{
		size = 0;
		if (lexeme[i] == '$')
		{
			size = get_idsize(lexeme, i);
			token_list_add(&subtkn_lst, WORD,\
					ft_substr(lexeme, i, size), size);
			i += (size - 1);
		}
		else
		{
			size = get_wordsize(lexeme, i);
			token_list_add(&subtkn_lst, WORD,\
					ft_substr(lexeme, i, size), size);
			i += (size - 1);
		}
		i++;
	}
	return (subtkn_lst);
}

