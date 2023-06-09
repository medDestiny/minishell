/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rdp.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 16:08:00 by mmisskin          #+#    #+#             */
/*   Updated: 2023/06/09 18:15:08 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	in_list(t_token *tok, t_node_type type)
{
	if (!is_connector(tok) && tok->type != L_PAREN
		&& tok->type != R_PAREN && tok->type != SPC)
	{
		if (type == REDIR_IN)
		{
			if (tok->type != REDIR_OUT && tok->type != APPEND)
				return (1);
			return (0);
		}
		else if (type == REDIR_OUT)
		{
			if (tok->type != REDIR_IN && tok->type != HEREDOC)
				return (1);
			return (0);
		}
		else if (type == WORD)
		{
			if (tok->type != REDIR_OUT && tok->type != APPEND
				&& tok->type != REDIR_IN && tok->type != HEREDOC)
				return (1);
			return (0);
		}
	}
	return (0);
}

int	build_list(t_token **ptr, t_token **tokens, t_node_type type)
{
	int	len;

	while (*tokens && in_list(*tokens, type))
	{
		len = ft_strlen((*tokens)->lexeme);
		if (token_list_add(ptr, (*tokens)->type, (*tokens)->lexeme, len) != 0)
			return (-1);
		*tokens = (*tokens)->next;
	}
	if (*tokens && (*tokens)->type == SPC)
		if (token_list_add(ptr, SPC, " ", 1) != 0)
			return (-1);
	return (0);
}

void	init_cmd_node(t_tree *cmd)
{
	cmd->type = T_CMD;
	cmd->cmd.global_out = 0;
	cmd->cmd.global_in = 0;
	cmd->cmd.subshell = 0;
	cmd->cmd.list = NULL;
	cmd->cmd.in = NULL;
	cmd->cmd.out = NULL;
}

int	parse_command(t_tree **root, t_token **tok)
{
	t_tree	*cmd;
	int		err;

	cmd = (t_tree *)ft_malloc(sizeof(t_tree), &g_gc);
	if (!cmd)
		return (-1);
	init_cmd_node(cmd);
	err = 0;
	while (err == 0 && *tok && (*tok)->type != PIPE && (*tok)->type != AND
		&& (*tok)->type != OR && (*tok)->type != L_PAREN
		&& (*tok)->type != R_PAREN)
	{
		if ((*tok)->type == REDIR_IN || (*tok)->type == HEREDOC)
			err = build_list(&cmd->cmd.in, tok, REDIR_IN);
		else if ((*tok)->type == REDIR_OUT || (*tok)->type == APPEND)
			err = build_list(&cmd->cmd.out, tok, REDIR_OUT);
		else
			err = build_list(&cmd->cmd.list, tok, WORD);
		if (*tok && (*tok)->type == SPC)
			*tok = (*tok)->next;
	}
	*root = cmd;
	return (err);
}

t_node_type	peek(t_token *tokens)
{
	while (tokens && tokens->type != PIPE && tokens->type != AND
		&& tokens->type != OR && tokens->type != L_PAREN
		&& tokens->type != R_PAREN)
		tokens = tokens->next;
	if (!tokens)
		return (END);
	return (tokens->type);
}

void	skip(t_token **tokens, t_node_type type)
{
	while (*tokens && ((*tokens)->type == type || (*tokens)->type == SPC))
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

	new = (t_tree *)ft_malloc(sizeof(t_tree), &g_gc);
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
	skip(tokens, SPC);
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
	skip(tokens, SPC);
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

void	update_redirs(t_tree *root, t_token *in, t_token *out)
{
	if (root->type == T_CMD)
	{
		root->cmd.subshell = 1;
		if (in && !root->cmd.in)
		{
			root->cmd.in = in;
			root->cmd.global_in = 1;
		}
		if (out && !root->cmd.out)
		{
			root->cmd.out = out;
			root->cmd.global_out = 1;
		}
	}
	else
	{
		update_redirs(root->node.lchild, in, out);
		update_redirs(root->node.rchild, in, out);
	}
}

int	add_group_redir(t_token *paren, t_tree *group)
{
	t_token	*input;
	t_token	*output;
	int		err;

	err = 0;
	input = NULL;
	output = NULL;
	skip(&paren, SPC);
	while (paren && paren->type != R_PAREN && paren->type != L_PAREN
		&& !is_connector(paren))
	{
		if (paren->type == REDIR_OUT || paren->type == APPEND)
			err = build_list(&output, &paren, REDIR_OUT);
		else if (paren->type == REDIR_IN || paren->type == HEREDOC)
			err = build_list(&input, &paren, REDIR_IN);
		else if (paren->type == SPC)
			paren = paren->next;
		if (err != 0)
			return (err);
	}
	update_redirs(group, input, output);
	return (err);
}

void	skip_redirs(t_token **tok)
{
	while (*tok)
	{
		if ((*tok)->type == HEREDOC || (*tok)->type == APPEND
			|| (*tok)->type == REDIR_IN || (*tok)->type == REDIR_OUT)
		{
			while (*tok && (*tok)->type != SPC && (*tok)->type != L_PAREN
				&& (*tok)->type != R_PAREN && (*tok)->type != AND
				&& (*tok)->type != OR && (*tok)->type != PIPE)
				*tok = (*tok)->next;
		}
		if (*tok && (*tok)->type == SPC)
			*tok = (*tok)->next;
		else
			break ;
	}
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
	if (!group)
		return (-1);
	else
		skip(tokens, R_PAREN);
	err = add_group_redir(*tokens, group);
	skip_redirs(tokens);
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
		if (peek(*tokens) == L_PAREN)
			skip_redirs(tokens);
		if (*tokens && (*tokens)->type == R_PAREN)
			break ;
		else if (*tokens && (*tokens)->type == L_PAREN)
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
