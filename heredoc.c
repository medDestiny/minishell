/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <mmisskin@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/08 02:04:37 by mmisskin          #+#    #+#             */
/*   Updated: 2023/08/08 03:47:41 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	heredoc(t_token *hdoc, int pipe)
{
	char	*line;
	t_token	*doc;

	doc = NULL;
	while (1)
	{
		line = readline("> ");
		if (!line || !ft_strcmp(line, hdoc->lexeme))
			break ;
		if (token_list_add(&doc, WORD, ft_strjoin(line, "\n"), ft_strlen(line) + 1) != 0)
			return ;
		free(line);
	}
	// expand if needed
	// join
	write(pipe, doc->lexeme, ft_strlen(doc->lexeme));
	t_token	*next;

	next = doc;
	while (next)
	{
		next = doc->next;
		free(doc->lexeme);
		free(doc);
		doc = next;
	}
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
