/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_redir.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaadiou <hlaadiou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/11 17:45:28 by hlaadiou          #+#    #+#             */
/*   Updated: 2023/08/15 16:35:23 by hlaadiou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	get_wordsize(char *lexeme, int i)
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
static int	get_idsize(char *lexeme, int i)
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

static int	get_subredir(t_token **lst, t_token *redir, int size, int i)
{
	char		*sub;
	t_node_type	type;

	type = redir->type;
	sub = ft_substr(redir->lexeme, i, size);
	if (!sub)
	{
		g_exit.status = ALLOCERR;
		return (-1);
	}
	if (i != 0 && (is_redir_in(type) || is_redir_out(type)))
	{
		if (type == RD_IN_DQ || type == RD_OUT_DQ || type == APPEND_DQ || type == HDOC)
			type = D_QUOTE;
		else if (type == RD_IN_WD || type == RD_OUT_WD || type == APPEND_WD || type == HDOC_EXP)
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

static t_token	*lst_last(t_token *lst)
{
	while (lst && lst->next)
		lst = lst->next;
	return (lst);
}

//Inserts a sublsit between lleft and lright of the original list;
//Warning: Use it wisely!
//the lleft and lright params should be consecutive nodes of the original list.
void	sublist_insert(t_token *sublst, t_token **lleft, t_token **lright)
{
	t_token	*last;

	if (!sublst)
		return ;
	last = lst_last(sublst);
	if (*lleft == NULL)
		*lleft = sublst;
	else
	{
		(*lleft)->next = sublst;
		sublst->prev = *lleft;
	}
	if (*lright == NULL)
		*lright = last;
	else
	{
		last->next = *lright;
		(*lright)->prev = last;
	}
}

//Subtokens of : 1-|<$"$""$USER$PATH"$NO'world'| 
//and            2-|<<$$$"$_"hi|
//are :          1-|$|->|"$USER"|->|"$PATH"|->|$NO|->|world|
//and            2-|$$|->|$|->|$_|->|hi|.
t_token	*redirtkn_split(t_token *redir)
{
	t_token	*subredir;
	int		size;
	int		i;
	int		j;

	i = -1;
	j = 0;
	subredir = NULL;
	if (redir->lexeme && !redir->lexeme[0] \
		&& token_list_add(&subredir, redir->type, redir->lexeme, 0) != 0)
		return (g_exit.status = ALLOCERR, NULL);
	while (redir->lexeme && redir->lexeme[++i])
	{
		if (redir->lexeme[i] == '$')
			size = get_subredir(&subredir, redir, \
					get_idsize(redir->lexeme, i), i);
		else
			size = get_subredir(&subredir, redir, \
					get_wordsize(redir->lexeme, i), i);
		if (size == -1)
			return (NULL);
		i += size;
	}
	return (subredir);
}

t_token	*redirlst_split(t_token *redir)
{
	t_token	*splitted;
	t_token	*right;
	t_token	*left;
	t_token	*tmp;

	splitted = NULL;
	left = NULL;
	while (redir)
	{
		right = NULL;
		tmp = NULL;
		left = lst_last(splitted);
		if (redir->type != S_QUOTE && redir->type != RD_IN_SQ \
			&& redir->type != RD_OUT_SQ && redir->type != APPEND_SQ)
			tmp = redirtkn_split(redir);
		else if (token_list_add(&tmp, redir->type, redir->lexeme, \
				ft_strlen(redir->lexeme)) != 0)
			return (NULL);
		if (tmp == NULL && g_exit.status == ALLOCERR)
			return (NULL);
		sublist_insert(tmp, &left, &right);
		if (!splitted)
			splitted = left;
		redir = redir->next;
	}
	return (splitted);
}
