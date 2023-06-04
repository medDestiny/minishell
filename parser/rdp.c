/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rdp.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 16:08:00 by mmisskin          #+#    #+#             */
/*   Updated: 2023/06/04 21:09:10 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	build_list(t_token **ptr, t_token **tokens)
{
	while (*tokens && (*tokens)->type != PIPE && (*tokens)->type != AND && (*tokens)->type != OR && (*tokens)->type != L_PAREN && (*tokens)->type != R_PAREN && (*tokens)->type != SPACE)
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

t_node_type	peek(t_token **tokens)
{
	while (*tokens && (*tokens)->type == SPACE)
		*tokens = (*tokens)->next;
	return ((*tokens)->type);
}

int	parse_pipe(t_tree **root, t_token **tokens)
{
	t_tree	*pipe;
	int		err;

	err = 0;
	pipe = (t_tree *)malloc(sizeof(t_tree));
	if (!pipe)
		return (-1);
	pipe->type = T_PIPE;
	pipe->node.lchild = *root;
	pipe->node.rchild = NULL;
	*tokens = (*tokens)->next;
	if (peek(tokens) == L_PAREN)
		err = parse_group(&pipe->node.rchild, tokens);
	else
		err = parse_command(&pipe->node.rchild, tokens);
	*root = pipe;
	return (err);
}

int	parse_condition(t_tree **root, t_token **tokens)
{
	t_tree	*condition;
	int		err;

	err = 0;
	condition = (t_tree *)malloc(sizeof(t_tree));
	if (!condition)
		return (-1);
	if ((*tokens)->type == OR)
		condition->type = T_OR;
	else
		condition->type = T_AND;
	condition->node.lchild = *root;
	*tokens = (*tokens)->next;
	//err = parse_command(&condition->node.rchild, tokens);
	*root = condition;
	(*root)->node.rchild = parser(*tokens);
	while (*tokens && peek(tokens) != L_PAREN && peek(tokens) != AND && peek(tokens) != OR)
		*tokens = (*tokens)->next;
	return (err);
}

int	parse_group(t_tree **root, t_token **tokens)
{
	t_tree	*group;
	int		err;

	err = 0;
	*tokens = (*tokens)->next;
	group = parser(*tokens);
	while (*tokens && peek(tokens) != R_PAREN)
		*tokens = (*tokens)->next;
	if (!*root)
		*root = group;
	else
		(*root)->node.rchild = group;
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
		if (tokens->type == R_PAREN)
			break ;
		if (tokens->type == L_PAREN)
			err = parse_group(&root, &tokens);
		else if (tokens->type == OR || tokens->type == AND)
			err = parse_condition(&root, &tokens);
		else if (tokens->type == PIPE)
			err = parse_pipe(&root, &tokens);
		else
			err = parse_command(&root, &tokens);
	}
	return (root);
}
