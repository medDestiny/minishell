/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rdp.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/29 16:08:00 by mmisskin          #+#    #+#             */
/*   Updated: 2023/05/31 17:22:52 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

//int	count_cmd(t_token *tok)
//{
//	int	count;
//
//	count = 0;
//	while (tok && tok->type != AND && tok->type != OR && tok->type != PIPE)
//	{
//		if (tok->type != REDIR_IN && tok->type != REDIR_OUT && tok->type != APPEND && tok->type != HEREDOC && tok->type != SPACE)
//		count++;
//		tok = tok->next;
//	}
//	return (count);
//}
//
//void	build_cmd(char **cmd, t_token **tokens)
//{
//	t_token	*tok;
//	t_token	*next;
//	int		i;
//
//	tok = *tokens;
//	i = 0;
//	while (tok && tok->type != AND && tok->type != OR && tok->type != PIPE)
//	{
//		if (tok->type != REDIR_IN && tok->type != REDIR_OUT && tok->type != APPEND && tok->type != HEREDOC && tok->type != SPACE)
//		{
//			next = tok->next;
//			cmd[i++] = ft_strdup(tok->lexeme);	
//			if (next)
//				next->prev = tok->prev;
//			tok = next;
//			continue ;
//		}
//		if (tok)
//			tok = tok->next;
//	}
//	while (tok && tok->prev)
//		tok = tok->prev;
//	*tokens = tok;
//}
//
//char	**build_cmd_vec(t_token **tok, int size)
//{
//	char	**cmd;
//
//	cmd = (char **)malloc((size + 1) * sizeof(char *));
//	if (!cmd)
//		return (NULL);
//	cmd[size] = NULL;
//	build_cmd(cmd, tok);
//	return (cmd);
//}

void	add_parent_node(t_tree **root, t_type type)
{
	t_tree	*child;

	child = *root;
	*root = malloc(sizeof(t_tree));
	if (!*root)
		return ;
	(*root)->type = type;
	(*root)->node.lchild = &child->cmd;
}

t_token	*build_cmd_vec(t_token **token)
{
	t_token	*new;
	t_token	*tok;
	t_token	*next;

	new = NULL;
	tok = *token;
	while (tok && tok->type != AND && tok->type != OR && tok->type != PIPE)
	{
		if (tok->type != REDIR_IN && tok->type != REDIR_OUT && tok->type != APPEND && tok->type != HEREDOC)
		{
			next = tok->next;
			if (token_list_add(&new, tok->type, tok->lexeme, ft_strlen(tok->lexeme)) != 0)
				return (NULL);
			if (next)
				next->prev = tok->prev;
			tok = next;
			continue ;
		}
		if (tok)
			tok = tok->next;
	}
	while (tok && tok->prev)
		tok = tok->prev;
	*token = tok;
	return (new);
}

t_tree	*parse_command(t_tree **root, t_token **tokens)
{
	t_tree	*cmdnode;

	cmdnode = (t_tree *)malloc(sizeof(t_tree));
	if (!cmdnode)
		return (NULL);
	cmdnode->type = T_CMD;
	cmdnode->cmd.tokens = build_cmd_vec(tokens);
	if (!*root)
		*root = cmdnode;
	else
		(*root)->node.rchild = &cmdnode->cmd;
	return (cmdnode);
}

void	parse_pipe(t_tree **root, t_token **tokens)
{
	add_parent_node();
}

t_tree	*parser(t_token **tokens)
{
	t_tree	*root;
	t_token	*ptr;

	root = NULL;
	ptr = *tokens;
	//if ((*tokens)->type == WORD)
	while (*tokens)
	{
		if ((*tokens)->type == PIPE)
			parse_pipe(&root, tokens);
		parse_command(&root, tokens);
		*tokens = (*tokens)->next;
	}
	return (root);
}
