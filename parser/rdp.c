/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rdp.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 16:08:00 by mmisskin          #+#    #+#             */
/*   Updated: 2023/06/03 22:02:52 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	build_list(t_token **ptr, t_token **tokens)
{
	while (*tokens && (*tokens)->type != PIPE && (*tokens)->type != AND && (*tokens)->type != OR && (*tokens)->type != L_PAREN && (*tokens)->type != SPACE)
	{
		if (token_list_add(ptr, (*tokens)->type, (*tokens)->lexeme, ft_strlen((*tokens)->lexeme)) != 0)
			return (-1);
		*tokens = (*tokens)->next;
	}
	if (*tokens && (*tokens)->type == SPACE)
		if (token_list_add(ptr, (*tokens)->type, (*tokens)->lexeme, ft_strlen((*tokens)->lexeme)) != 0)
			return (-1);
	return (0);
}

int	parse_command(t_tree **root, t_token **tokens)
{
	t_tree	*cmd;
	int		err;
	
	cmd = (t_tree *)malloc(sizeof(t_tree));
	if (!cmd)
		return (-1);
	cmd->type = T_CMD;
	cmd->cmd.list = NULL;
	cmd->cmd.in = NULL;
	cmd->cmd.out = NULL;
	err = 0;
	while (*tokens && (*tokens)->type != PIPE && (*tokens)->type != AND && (*tokens)->type != OR && (*tokens)->type != L_PAREN)
	{
		if ((*tokens)->type == REDIR_IN || (*tokens)->type == HEREDOC)
			err = build_list(&cmd->cmd.in, tokens);
		else if ((*tokens)->type == REDIR_OUT || (*tokens)->type == APPEND)
			err = build_list(&cmd->cmd.out, tokens);
		else
			err = build_list(&cmd->cmd.list, tokens);
		if (err != 0)
			break ;
		if (*tokens)
			*tokens = (*tokens)->next;
	}
	*root = cmd;
	return (err);
}

t_tree	*parser(t_token *tokens)
{
	t_tree	*root;
	int		err;

	root = NULL;
	err = 0;
	while (tokens)
	{
		//if (tokens->type == L_PAREN)
		//	parse_group();
		//else if (tokens->type == OR || tokens->type == AND)
		//	parse_condition();
		//else if (tokens->type == PIPE)
		//	parse_pipe();
		//else
		err = parse_command(&root, &tokens);
			break ;
	}
	return (root);
}
