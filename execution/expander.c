/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaadiou <hlaadiou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/15 08:13:22 by hlaadiou          #+#    #+#             */
/*   Updated: 2023/07/18 20:50:23 by hlaadiou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../minishell.h"

void	tkn_join(t_token *token, t_token **new_tknlst)
{
	// TODO :
	
	// Create a function that checks the list and joins the tokens
	// from the same family until you hit a space node in a new list and continue.
}

t_token	*tknlst_update(t_token *token)
{
	char	*id;
	char	*new_lexeme;
	char	**sub_tkns;

	sub_tkns = tkn_split(token->lexeme);
}

t_token	*list_expand(t_token *tokens)
{
	t_token	*new_tknlst;
	t_token	*token;

	token = tokens;
	if (!token)
		return (NULL);
	while (token)
	{
		if (token->type == D_QUOTE || token->type == WORD)
			tknlst_update(token);
		token = token->next;
	}
	tkn_join(tokens, &new_tknlst);
	return (new_tknlst);
}

t_token	*rdir_expand(t_token *tokens_io)
{
	t_token	*new_tknlst;
	t_token	*token;

	token = tokens_io;
	if (!token)
		return (NULL);
	while (token)
	{
	}
}

void	node_expand(t_cmd *cmd_node)
{
	if (cmd_node)
	{
		cmd_node->list = list_expand(cmd_node->list);
		cmd_node->in = rdir_expand(cmd_node->in);
		cmd_node->out = rdir_expand(cmd_node->out);
	}
	return ;
}
