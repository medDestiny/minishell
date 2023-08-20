/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_token.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaadiou <hlaadiou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/17 13:51:00 by hlaadiou          #+#    #+#             */
/*   Updated: 2023/08/11 19:05:43 by hlaadiou         ###   ########.fr       */
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
		if (in_set(lexeme[i], "$!?@#*-") || ft_isdigit(lexeme[i]) == 1)
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

int	get_subtkn(t_token **lst, t_token *tkn, int size, int i)
{
	char		*sub;
	t_node_type	type;

	type = tkn->type;
	sub = ft_substr(tkn->lexeme, i, size);
	if (!sub)
	{
		g_exit.status = ALLOCERR;
		return (-1);
	}
	if (i != 0 && (is_redir_in(type) || is_redir_out(type)))
	{
		if (type == RD_IN_DQ || type == RD_OUT_DQ || type == APPEND_DQ)
			type = D_QUOTE;
		else if (type == RD_IN_WD || type == RD_OUT_WD || type == APPEND_WD)
			type = WORD;
	}
	if (token_list_add(lst, type, sub, size) != 0)
	{
		free(sub);
		return (-1);
	}
	free(sub);
	return (size - 1);
}

//Subtokens of : |hii$hi?hello$$$123_cv-ls$PATH$hh$$hello!$?|
//|hii|->|$hi|->|?hello|->|$$|->|$1|->|23_cv-ls|->
//|$PATH|->|$hh|->|$$|->|hello!|->|$?|
t_token	*tkn_split(t_token *tkn)
{
	t_token	*subtkn_lst;
	int		size;
	int		i;
	int		j;

	i = 0;
	j = 0;
	subtkn_lst = NULL;
	if (!tkn->lexeme)
		return (NULL);
	while (tkn->lexeme[i])
	{
		if (tkn->lexeme[i] == '$')
			size = get_subtkn(&subtkn_lst, tkn, \
					get_idsize(tkn->lexeme, i), i);
		else
			size = get_subtkn(&subtkn_lst, tkn, \
					get_wordsize(tkn->lexeme, i), i);
		if (size == -1)
			return (NULL);
		i += size;
		i++;
	}
	return (subtkn_lst);
}
