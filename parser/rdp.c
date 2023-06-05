/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rdp.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 16:08:00 by mmisskin          #+#    #+#             */
/*   Updated: 2023/06/05 15:23:41 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	build_list(t_token **ptr, t_token **tokens)
{
	while (*tokens && (*tokens)->type != PIPE && (*tokens)->type != AND && (*tokens)->type != OR
		&& (*tokens)->type != L_PAREN && (*tokens)->type != R_PAREN && (*tokens)->type != SPACE)
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
	while (err == 0 && *tokens && (*tokens)->type != PIPE && (*tokens)->type != AND
		&& (*tokens)->type != OR && (*tokens)->type != L_PAREN && (*tokens)->type != R_PAREN)
	{
		if ((*tokens)->type == REDIR_IN || (*tokens)->type == HEREDOC)
			err = build_list(&cmd->cmd.in, tokens);
		else if ((*tokens)->type == REDIR_OUT || (*tokens)->type == APPEND)
			err = build_list(&cmd->cmd.out, tokens);
		else
			err = build_list(&cmd->cmd.list, tokens);
		if (*tokens && (*tokens)->type == SPACE)
			*tokens = (*tokens)->next;
	}
	*root = cmd;
	return (err);
}

t_node_type	peek(t_token *tokens)
{
	while (tokens && tokens->type != PIPE && tokens->type != AND && tokens->type != OR && tokens->type != L_PAREN && tokens->type != R_PAREN)
		tokens = tokens->next;
	if (!tokens)
		return (END);
	return (tokens->type);
}

void	skip(t_token **tokens, t_node_type type)
{
	while (*tokens && ((*tokens)->type == type || (*tokens)->type == SPACE))
	{
		if ((*tokens)->type == type)
		{
			*tokens = (*tokens)->next;
			break ;
		}
		*tokens = (*tokens)->next;
	}
}

t_tree	*new_tree_node(t_type type, t_tree *left, t_tree *right)
{
	t_tree	*new;

	new = (t_tree *)malloc(sizeof(t_tree));
	if (!new)
		return (NULL);
	new->type = type;
	new->node.lchild = left;
	new->node.rchild = right;
	return (new);
}

int	parse_pipeline(t_tree **root, t_token **tokens)
{
	t_tree	*cmd;
	int		err;

	err = 0;
	cmd = NULL;
	skip(tokens, SPACE);
	if (*tokens && (*tokens)->type != R_PAREN && (*tokens)->type != PIPE)
		err = parse_command(root, tokens);
	while (1)
	{
		if (peek(*tokens) == PIPE)
		{
			skip(tokens, PIPE);
			err = parse_command(&cmd, tokens);
			if (err < 0)
				return (err);
			*root = new_tree_node(T_PIPE, *root, cmd);
			if (!*root)
				return (-1);
		}
		else
			return (err);
	}
	return (err);
}

t_type	condition_node_type(t_token **tokens)
{
	t_node_type	type;

	type = peek(*tokens);
	skip(tokens, type);
	if (type == PIPE)
		return (T_PIPE);
	else if (type == OR)
		return (T_OR);
	else if (type == AND)
		return (T_AND);
	else
		return (T_CMD);
}

int	parse_condition(t_tree **root, t_token **tokens)
{
	t_tree		*pipeline;
	t_type		type;
	int			err;

	err = 0;
	pipeline = NULL;
	skip(tokens, SPACE);
	if (*tokens && (*tokens)->type != OR && (*tokens)->type != AND)
		err = parse_pipeline(root, tokens);
	while (1)
	{
		if (peek(*tokens) == OR || peek(*tokens) == AND)
		{
			type = condition_node_type(tokens);
			err = parse_pipeline(&pipeline, tokens);
			if (err < 0)
				return (err);
			*root = new_tree_node(type, *root, pipeline);
			if (!*root)
				return (-1);
		}
		else
			return (err);
	}
	return (err);
}

int	parse_group(t_tree **root, t_token **tokens)
{
	t_tree	*group;
	int		err;

	err = 0;
	group = NULL;
	skip(tokens, L_PAREN);
	group = parser(tokens);
	if (!*root)
		*root = group;
	else
		(*root)->node.rchild = group;
	if (!group || peek(*tokens) != R_PAREN)
	{
		printf("minishell: syntax error near unexpected token `)'\n");
		return (-1);
	}
	else
		skip(tokens, R_PAREN);
	return (err);
}

t_tree	*parser(t_token **tokens)
{
	t_tree	*root;
	int		err;

	err = 0;
	root = NULL;
	while (*tokens)
	{
		if ((*tokens)->type == R_PAREN)
			break ;
		else if ((*tokens)->type == L_PAREN)
			err = parse_group(&root, tokens);
		else if (peek(*tokens) == OR || peek(*tokens) == AND)
			err = parse_condition(&root, tokens);
		else
			err = parse_pipeline(&root, tokens);
		if (err != 0)
			return (NULL);
	}
	return (root);
}
