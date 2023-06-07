/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/22 18:31:45 by mmisskin          #+#    #+#             */
/*   Updated: 2023/06/07 12:06:39 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_token	*new_token_node(t_node_type type, char *content, size_t size)
{
	t_token	*node;

	node = (t_token *)malloc(sizeof(t_token));
	if (!node)
		return (NULL);
	node->lexeme = (char *)malloc((size + 1) * sizeof(char));
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

int	check_tokens(t_token **tokens, char *cmdline, char *limit);

int	check_word(t_token **tokens, char *cmdline, char *limit);

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
		printf("minishell: syntax error near unexpected token: ||\n");
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
		printf("minishell: syntax error near unexpected token: &&\n");
		return (-1);
	}
	//if (!*tokens || !cmdline[2] || i != 2 || in_set(cmdline[2], "()&|"))
	//{
	//	printf("minishell: syntax error near unexpected token ");
	//	if (!*tokens)
	//		printf("`&&'\n");
	//	else if (!cmdline[2])
	//		printf("newline\n");
	//	else
	//	{
	//		i = 2;
	//		while (cmdline[i] && cmdline[i] == ' ')
	//			i++;
	//		while (cmdline[i] && cmdline[i] != ' ' && i < 4)
	//			printf("%c", cmdline[i++]);
	//		printf("\n");
	//	}
	//	return (-1);
	//}
	if (token_list_add(tokens, AND, cmdline, 2) != 0)
		return (-1);
	return (2);
}

int	check_pipe(t_token **tokens, char *cmdline)
{
	if (!*tokens)
	{
		printf("minishell: syntax error near unexpected token: |\n");
		return (-1);
	}
	if (token_list_add(tokens, PIPE, cmdline, 1) != 0)
		return (-1);
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
		printf("minishell: syntax error near unexpected token: ");
		if (!cmdline[start])
			printf("newline\n");
		else
			printf("%c\n", cmdline[start]);
		return (-1);
	}
	while (cmdline[start] && in_set(cmdline[start], BLANK))
		start++;
	size = check_tokens(tokens, cmdline + start, SEPARATOR);
	if (size < 0)
		return (-1);
	last = lst_last(*tokens);
	last->type = HEREDOC;
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
		printf("minishell: syntax error near unexpected token: ");
		if (!cmdline[start])
			printf("newline\n");
		else
			printf("%c\n", cmdline[start]);
		return (-1);
	}
	while (cmdline[start] && in_set(cmdline[start], BLANK))
		start++;
	size = check_tokens(tokens, cmdline + start, SEPARATOR);
	if (size < 0)
		return (-1);
	last = lst_last(*tokens);
	last->type = APPEND;
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
		printf("minishell: syntax error near unexpected token: ");
		if (!cmdline[start])
			printf("newline\n");
		else
			printf("%c\n", cmdline[start]);
		return (-1);
	}
	while (cmdline[start] && in_set(cmdline[start], BLANK))
		start++;
	size = check_tokens(tokens, cmdline + start, SEPARATOR);
	if (size < 0)
		return (-1);
	last = lst_last(*tokens);
	last->type = REDIR_IN;
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
		printf("minishell: syntax error near unexpected token: ");
		if (!cmdline[start])
			printf("newline\n");
		else
			printf("%c\n", cmdline[start]);
		return (-1);
	}
	while (cmdline[start] && in_set(cmdline[start], BLANK))
		start++;
	size = check_tokens(tokens, cmdline + start, SEPARATOR);
	if (size < 0)
		return (-1);
	last = lst_last(*tokens);
	last->type = REDIR_OUT;
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
		printf("minishell: unclosed single quotes\n");
		return (-1);
	}
	size = check_word(tokens, cmdline, "'");
	if (size < 0)
		return (-1);
	if (size)
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
		printf("minishell: unclosed double quotes\n");
		return (-1);
	}
	size = check_word(tokens, cmdline, "\"");
	if (size < 0)
		return (-1);
	if (size)
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
			if (token_list_add(&tokens, SPACE, " ", 1) != 0)
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
