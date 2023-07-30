/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaadiou <hlaadiou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/15 08:13:22 by hlaadiou          #+#    #+#             */
/*   Updated: 2023/07/30 18:35:23 by hlaadiou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../minishell.h"

t_token	*tkn_join(t_token *lst)
{
	t_token	*joined;
	char	*newlex;
	char	*tmp;

	joined = NULL;
	while (lst)
	{
		newlex = NULL;
		while (lst && lst->type != SPC)
		{
			tmp = newlex;
			newlex = ft_strjoin(tmp, lst->lexeme);
			free(tmp);
			lst = lst->next;
		}
		if (newlex)
			token_list_add(&joined, WORD, newlex, ft_strlen(newlex));
		if (lst)
		{
			token_list_add(&joined, lst->type, lst->lexeme, 1);
			lst = lst->next;
		}
		free(newlex);
	}
	return (joined);
}

// Update the token if it is of type WORD or D_QUOTE
// into a new expanded token added to the 'newtknlst'.
void	tkn_update(t_token **newlst, t_token *lst, t_env *env)
{
	char	*var;
	t_token	*sub;

	sub = tkn_split(lst);
	if (!sub)
		return ;
	while (sub)
	{
		var = sub->lexeme;
		if (var && *var == '$' && *(var + 1))
			var = get_env_value(env, (var + 1));
		if (var)
			token_list_add(newlst, sub->type, var, ft_strlen(var));
		sub = sub->next;
	}
	return ;
}

t_token	*list_expand(t_token *tokens, t_env *env)
{
	t_token	*token;
	t_token	*newtknlst;

	token = tokens;
	newtknlst = NULL;
	while (token)
	{
		if (token->type == WORD || token->type == D_QUOTE)
			tkn_update(&newtknlst, token, env);
		else
			token_list_add(&newtknlst, token->type, token->lexeme, \
					ft_strlen(token->lexeme));
		token = token->next;
	}
	newtknlst = tkn_join(newtknlst);
	return (newtknlst);
}

//t_token	*rdir_expand(t_token *tokens_io)
//{
//}

void	node_expand(t_cmd *cmd_node, t_env *env)
{
	if (cmd_node)
	{
		cmd_node->list = list_expand(cmd_node->list, env);
		//cmd_node->in = rdir_expand(cmd_node->in);
		//cmd_node->out = rdir_expand(cmd_node->out);
	}
	return ;
}
