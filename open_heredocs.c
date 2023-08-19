/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_heredocs.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <mmisskin@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/19 21:08:25 by mmisskin          #+#    #+#             */
/*   Updated: 2023/08/19 23:39:18 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*get_hdoc_delim(t_token **redir)
{
	t_token	*delim;
	char	*var;

	delim = NULL;
	while (1)
	{
		var = hdoc_expand(*redir);
		if (!var)
		{
			*redir = (*redir)->next;
			continue ;
		}
		if (token_list_add(&delim, (*redir)->type, var, ft_strlen(var)) != 0)
			return (NULL);
		*redir = (*redir)->next;
		if (!*redir || is_redir_in((*redir)->type) || is_redir_out((*redir)->type))
			break ;
	}
	if (delim)
		delim = redir_join(delim);
	return (delim);
}

int	update_hdoc_fd(t_tree *cmd, t_env *env)
{
	t_token	*redir;
	t_token	*delim;

	delim = NULL;
	redir = cmd->cmd.redir;
	while (redir)
	{
		if (redir->type == HDOC || redir->type == HDOC_EXP)
		{
			if (cmd->cmd.hdoc != -1)
				close(cmd->cmd.hdoc);
			delim = get_hdoc_delim(&redir);
			cmd->cmd.hdoc = open_heredoc(delim, env);
			if (cmd->cmd.hdoc == -1)
				return (1);
		}
		if (redir && redir->type != HDOC && redir->type != HDOC_EXP)
			redir = redir->next;
	}
	return (0);
}

int	get_hdoc_fd(t_tree *cmd, t_env *env)
{
	t_token	*redir;
	t_token	*delim;
	int		fd;

	delim = NULL;
	while (cmd->type != T_CMD && cmd->type != S_CMD)
		cmd = cmd->node.lchild;
	redir = cmd->cmd.sub_redir;
	while (redir)
	{
		if (redir->type == HDOC || redir->type == HDOC_EXP)
			delim = get_hdoc_delim(&redir);
		fd = open_heredoc(delim, env);
		if (fd == -1)
			return (-1);
		if (redir)
			redir = redir->next;
	}
	return (fd);
}

int	open_heredocs(t_tree *root, t_env *env)
{
	if (!root)
		return (1);
	if (root->type != T_CMD && !is_subshell(root->type))
	{
		if (open_heredocs(root->node.lchild, env) == 1)
			return (1);
		if (open_heredocs(root->node.rchild, env) == 1)
			return (1);
	}
	else
		if (update_hdoc_fd(root, env) == 1)
			return (1);
	return (0);
}
