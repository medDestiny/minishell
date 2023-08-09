/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <mmisskin@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/08 02:04:37 by mmisskin          #+#    #+#             */
/*   Updated: 2023/08/09 16:06:10 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*read_heredoc(char *delim)
{
	t_token	*doc;
	char	*line;
	char	*joined;

	doc = NULL;
	while (1)
	{
		line = readline("> ");
		if (!line || !ft_strcmp(line, delim))
		{
			free(line);
			break ;
		}
		joined = ft_strjoin(line, "\n");
		if (token_list_add(&doc, WORD, joined, ft_strlen(line) + 1) != 0)
			return (NULL);
		free(joined);
		free(line);
	}
	return (doc);
}

void	heredoc(t_token *hdoc, int pipe)
{
	t_token	*doc;

	doc = read_heredoc(hdoc->lexeme);
	if (hdoc->type == HDOC_EXP)
	{
		// expand
	}
	while (doc)
	{
		write(pipe, doc->lexeme, ft_strlen(doc->lexeme));
		doc = doc->next;
	}
	clean_all();
}

void	handle_heredoc(t_token *hdoc, int pipe)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		ft_putstr_fd("fork failed\n", STDERR_FILENO);
		return ;
	}
	if (pid == 0)
	{
		heredoc(hdoc, pipe);
		exit(0);
	}
	else
		wait(NULL);
}
