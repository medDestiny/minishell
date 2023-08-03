/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/22 18:31:45 by mmisskin          #+#    #+#             */
/*   Updated: 2023/08/03 16:25:45 by hlaadiou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_token	*new_token_node(t_node_type type, char *content, size_t size)
{
	t_token	*node;

	node = (t_token *)ft_malloc(sizeof(t_token), &g_gc);
	if (!node)
		return (NULL);
	node->lexeme = (char *)ft_malloc((size + 1) * sizeof(char), &g_gc);
	if (!node->lexeme)
		return (NULL);
	ft_strlcpy(node->lexeme, content, size + 1);
	node->type = type;
	node->next = NULL;
	return (node);
}

int	token_list_add(t_token **list, t_node_type type, char *content, size_t size)
{
	t_token	*tmp;

	tmp = *list;
	if (!tmp)
	{
		*list = new_token_node(type, content, size);
		tmp = *list;
		if (tmp)
			tmp->prev = NULL;
	}
	else
	{
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new_token_node(type, content, size);
		tmp->next->prev = tmp;
		tmp = tmp->next;
	}
	if (!tmp)
		return (1);
	return (0);
}

t_token	*lst_last(t_token *tokens)
{
	if (!tokens)
		return (NULL);
	while (tokens->next)
		tokens = tokens->next;
	return (tokens);
}

int	in_set(char c, char *set)
{
	int	i;

	i = 0;
	while (set[i])
	{
		if (set[i] == c)
			return (1);
		i++;
	}
	return (0);
}

int	check_parenthesis(t_token **tokens, char *cmdline)
{
	if (cmdline[0] == '(')
	{
		if (token_list_add(tokens, L_PAREN, cmdline, 1) != 0)
			return (-1);
	}
	else if (cmdline[0] == ')')
	{
		if (token_list_add(tokens, R_PAREN, cmdline, 1) != 0)
			return (-1);
	}
	return (1);
}

int	check_or(t_token **tokens, char *cmdline)
{
	if (!*tokens)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token: ||\n",
			STDERR_FILENO);
		return (-1);
	}
	if (token_list_add(tokens, OR, cmdline, 2) != 0)
		return (-1);
	return (2);
}

int	check_and(t_token **tokens, char *cmdline)
{
	int		i;

	i = 0;
	while (cmdline[i] && cmdline[i] == '&')
		i++;
	if (i < 2)
	{
		if (token_list_add(tokens, WORD, cmdline, i) != 0)
			return (-1);
		return (i);
	}
	if (!*tokens)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token: &&\n",
			STDERR_FILENO);
		return (-1);
	}
	if (token_list_add(tokens, AND, cmdline, 2) != 0)
		return (-1);
	return (2);
}

int	check_pipe(t_token **tokens, char *cmdline)
{
	if (!*tokens)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token: |\n",
			STDERR_FILENO);
		return (-1);
	}
	if (token_list_add(tokens, PIPE, cmdline, 1) != 0)
		return (-1);
	return (1);
}

int	redir_syntax_error(char *cmdline)
{
	if (!cmdline[0])
		print_syntax_error(UNEX_TOK, "newline");
	else if (!ft_strncmp(cmdline, "||", 2))
		print_syntax_error(UNEX_TOK, "||");
	else if (cmdline[0] == '|')
		print_syntax_error(UNEX_TOK, "|");
	else if (!ft_strncmp(cmdline, "&&", 2))
		print_syntax_error(UNEX_TOK, "&&");
	else if (cmdline[0] == '&')
		return (0);
	else if (cmdline[0] == '(')
		print_syntax_error(UNEX_PAREN, NULL);
	else if (cmdline[0] == ')')
		print_syntax_error(UNEX_RPAR, NULL);
	else if (!ft_strncmp(cmdline, ">>", 2))
		print_syntax_error(UNEX_TOK, ">>");
	else if (cmdline[0] == '>')
		print_syntax_error(UNEX_TOK, ">");
	else if (!ft_strncmp(cmdline, "<<", 2))
		print_syntax_error(UNEX_TOK, "<<");
	else if (cmdline[0] == '<')
		print_syntax_error(UNEX_TOK, "<");
	return (1);
}

int	check_heredoc(t_token **tokens, char *cmdline)
{
	int		start;
	int		size;
	t_token	*last;

	start = 2;
	while (cmdline[start] && in_set(cmdline[start], BLANK))
		start++;
	if (!cmdline[start] || in_set(cmdline[start], HDOC_SEP))
	{
		if (redir_syntax_error(&cmdline[start]) != 0)
			return (-1);
	}
	size = check_tokens(tokens, cmdline + start, SEPARATOR);
	if (size < 0)
		return (-1);
	last = lst_last(*tokens);
	if (last->type != WORD)
		last->type = HDOC;
	else
		last->type = HDOC_EXP;
	return (start + size);
}

int	check_append(t_token **tokens, char *cmdline)
{
	t_token	*last;
	int		start;
	int		size;

	start = 2;
	size = 0;
	while (cmdline[start] && in_set(cmdline[start], BLANK))
		start++;
	if (!cmdline[start] || in_set(cmdline[start], HDOC_SEP))
	{
		if (redir_syntax_error(&cmdline[start]) != 0)
			return (-1);
	}
	size = check_tokens(tokens, cmdline + start, SEPARATOR);
	if (size < 0)
		return (-1);
	last = lst_last(*tokens);
	if (last->type == S_QUOTE)
		last->type = APPEND;
	else
		last->type = APPEND_EXP;
	return (start + size);
}

int	check_infile(t_token **tokens, char *cmdline)
{
	t_token	*last;
	int		start;
	int		size;

	start = 1;
	size = 0;
	while (cmdline[start] && in_set(cmdline[start], BLANK))
		start++;
	if (!cmdline[start] || in_set(cmdline[start], HDOC_SEP))
	{
		if (redir_syntax_error(&cmdline[start]) != 0)
			return (-1);
	}
	size = check_tokens(tokens, cmdline + start, SEPARATOR);
	if (size < 0)
		return (-1);
	last = lst_last(*tokens);
	if (last->type == S_QUOTE)
		last->type = RD_IN;
	else
		last->type = RD_IN_EXP;
	return (start + size);
}

int	check_outfile(t_token **tokens, char *cmdline)
{
	t_token	*last;
	int		start;
	int		size;

	start = 1;
	size = 0;
	while (cmdline[start] && in_set(cmdline[start], BLANK))
		start++;
	if (!cmdline[start] || in_set(cmdline[start], HDOC_SEP))
	{
		if (redir_syntax_error(&cmdline[start]) != 0)
			return (-1);
	}
	size = check_tokens(tokens, cmdline + start, SEPARATOR);
	if (size < 0)
		return (-1);
	last = lst_last(*tokens);
	if (last->type == S_QUOTE)
		last->type = RD_OUT;
	else
		last->type = RD_OUT_EXP;
	return (start + size);
}

int	check_redirection(t_token **tokens, char *cmdline)
{
	if (!ft_strncmp(cmdline, "<<", 2))
		return (check_heredoc(tokens, cmdline));
	else if (!ft_strncmp(cmdline, ">>", 2))
		return (check_append(tokens, cmdline));
	else if (cmdline[0] == '<')
		return (check_infile(tokens, cmdline));
	else
		return (check_outfile(tokens, cmdline));
}

int	check_single_quotes(t_token **tokens, char *cmdline)
{
	t_token	*last;
	int		i;
	int		size;

	i = 0;
	while (cmdline[i] && cmdline[i] != '\'')
		i++;
	if (!cmdline[i])
	{
		ft_putstr_fd("minishell: unclosed single quotes\n", STDERR_FILENO);
		return (-1);
	}
	size = check_word(tokens, cmdline, "'");
	if (size < 0)
		return (-1);
	else
	{
		last = lst_last(*tokens);
		last->type = S_QUOTE;
	}
	return (i + 2);
}

int	check_double_quotes(t_token **tokens, char *cmdline)
{
	t_token	*last;
	int		i;
	int		size;

	i = 0;
	while (cmdline[i] && cmdline[i] != '"')
		i++;
	if (!cmdline[i])
	{
		ft_putstr_fd("minishell: unclosed double quotes\n", STDERR_FILENO);
		return (-1);
	}
	size = check_word(tokens, cmdline, "\"");
	if (size < 0)
		return (-1);
	else
	{
		last = lst_last(*tokens);
		last->type = D_QUOTE;
	}
	return (i + 2);
}

int	check_word(t_token **tokens, char *cmdline, char *limit)
{
	int			size;

	size = 0;
	while (cmdline[size] && !in_set(cmdline[size], limit))
		size++;
	if (token_list_add(tokens, WORD, cmdline, size) != 0)
		return (-1);
	return (size);
}

int	check_tokens(t_token **tokens, char *cmdline, char *limit)
{
	int	size;

	size = 0;
	if (cmdline[0] == '(' || cmdline[0] == ')')
		size = check_parenthesis(tokens, cmdline);
	else if (cmdline[0] == '|')
	{
		if (cmdline[1] == '|')
			size = check_or(tokens, cmdline);
		else
			size = check_pipe(tokens, cmdline);
	}
	else if (cmdline[0] == '&')
		size = check_and(tokens, cmdline);
	else if (in_set(cmdline[0], "<>"))
		size = check_redirection(tokens, cmdline);
	else if (cmdline[0] == '\'')
		size = check_single_quotes(tokens, cmdline + 1);
	else if (cmdline[0] == '"')
		size = check_double_quotes(tokens, cmdline + 1);
	else
		size = check_word(tokens, cmdline, limit);
	return (size);
}

t_token	*lexer(char *cmdline)
{
	t_token	*tokens;
	int		i;
	int		skip;

	i = 0;
	tokens = NULL;
	while (cmdline[i])
	{
		skip = 0;
		if (cmdline[i] && in_set(cmdline[i], BLANK))
		{
			while (cmdline[i] && in_set(cmdline[i], BLANK))
				i++;
			if (token_list_add(&tokens, SPC, " ", 1) != 0)
				return (NULL);
		}
		skip = check_tokens(&tokens, cmdline + i, SEPARATOR);
		if (skip < 0)
			return (NULL);
		i += skip;
	}
	check_syntax(&tokens);
	return (tokens);
}
