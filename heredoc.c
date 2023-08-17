/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <mmisskin@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/08 02:04:37 by mmisskin          #+#    #+#             */
/*   Updated: 2023/08/17 05:18:11 by hlaadiou         ###   ########.fr       */
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

void	heredoc(t_token *hdoc, int *pipe)
{
	t_token	*doc;

	default_signals();
	doc = read_heredoc(hdoc->lexeme);
	if (hdoc->type == HDOC_EXP)
	{
		// expand
	}
	while (doc)
	{
		write(pipe[1], doc->lexeme, ft_strlen(doc->lexeme));
		doc = doc->next;
	}
	close(pipe[0]);
	close(pipe[1]);
	clean_all();
	exit(0);
}

int	open_heredoc(t_token *hdoc)
{
	int		end[2];
	pid_t	pid;

	if (pipe(end) == -1)
	{
		ft_putstr_fd("Heredoc pipe failed\n", STDERR_FILENO);
		return (-1);
	}
	pid = fork();
	if (pid == -1)
	{
		ft_putstr_fd("fork failed\n", STDERR_FILENO);
		return (-1);
	}
	if (pid == 0)
		heredoc(hdoc, end);
	else
	{
		ignore_signals();
		waitpid(pid, 0, 0);
		default_signals();
		//signal_interrupter();
		close(end[1]);
	}
	return (end[0]);
}
