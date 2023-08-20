/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open_heredocs.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <mmisskin@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/19 21:08:25 by mmisskin          #+#    #+#             */
/*   Updated: 2023/08/20 02:50:54 by mmisskin         ###   ########.fr       */
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
		if (!*redir \
			|| is_redir_in((*redir)->type) || is_redir_out((*redir)->type))
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
	fd = -1;
	while (cmd && cmd->type != T_CMD && cmd->type != S_CMD)
		cmd = cmd->node.lchild;
	redir = cmd->cmd.sub_redir;
	while (redir)
	{
		if (redir->type == HDOC || redir->type == HDOC_EXP)
		{
			if (fd != -1)
				close(fd);
			delim = get_hdoc_delim(&redir);
			fd = open_heredoc(delim, env);
			if (fd == -1)
				return (-2);
		}
		if (redir && redir->type != HDOC && redir->type != HDOC_EXP)
			redir = redir->next;
	}
	return (fd);
}

void	update_subshell_hdoc(t_tree *root, int subsh_hdoc)
{
	if (root->type != T_CMD && root->type != S_CMD)
	{
		update_subshell_hdoc(root->node.lchild, subsh_hdoc);
		update_subshell_hdoc(root->node.rchild, subsh_hdoc);
	}
	else
		root->cmd.sub_hdoc = subsh_hdoc;
}

int	open_heredocs(t_tree *root, t_env *env)
{
	int	sub_hdoc;

	if (!root)
		return (1);
	if (is_subshell(root->type))
	{
		sub_hdoc = get_hdoc_fd(root, env);
		if (sub_hdoc == -2)
			return (1);
		update_subshell_hdoc(root, sub_hdoc);
	}
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
