/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 20:07:24 by mmisskin          #+#    #+#             */
/*   Updated: 2023/06/07 17:08:32 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	print_error(char *s1, char *s2)
{
	ft_putstr_fd(s1, STDERR_FILENO);
	if (s2)
		ft_putstr_fd(s2, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

int	is_connector(t_token *tok)
{
	if (tok->type == OR || tok->type == AND || tok->type == PIPE)
		return (1);
	return (0);
}

int	check_before_con(t_token *tok)
{
	t_token	*ptr;

	ptr = tok->prev;
	while (ptr)
	{
		if (ptr->type == L_PAREN)
		{
			if (tok->type == PIPE)
				return (UNEX_PIPE);
			else if (tok->type == OR)
				return (UNEX_OR);
			else if (tok->type == AND)
				return (UNEX_AND);
		}
		else if (ptr->type == SPC)
			ptr = ptr->prev;
		else
			break ;
	}
	return (0);
}

int	check_after_con(t_token *ptr)
{
	while (ptr)
	{
		if (ptr->type == PIPE)
			return (UNEX_PIPE);
		else if (ptr->type == OR)
			return (UNEX_OR);
		else if (ptr->type == AND)
			return (UNEX_AND);
		else if (ptr->type == R_PAREN)
			return (UNEX_RPAR);
		else if (ptr->type == SPC)
			ptr = ptr->next;
		else
			break ;
	}
	if (!ptr)
		return (UNEX_NL);
	return (0);
}

int	check_connector(t_token **tok)
{
	int		err;

	err = check_before_con(*tok);
	if (err != 0)
		return (err);
	err = check_after_con((*tok)->next);
	if (err != 0)
		return (err);
	return (0);
}

int	check_for_closing_paren(t_token **tok)
{
	while (*tok)
	{
		if ((*tok)->type == R_PAREN)
			return (0);
		*tok = (*tok)->next;
	}
	return (UNCLOSED);
}

int	is_cmd(t_token *tok)
{
	if (tok->type != REDIR_IN && tok->type != REDIR_OUT
		&& tok->type != HEREDOC && tok->type != APPEND
		&& !is_connector(tok))
		return (1);
	return (0);
}

int	check_left(t_token *paren)
{
	paren = paren->prev;
	if (paren && paren->type == SPC)
		paren = paren->prev;
	if (paren && paren->type != L_PAREN && !is_connector(paren))
		return (UNEX_PAREN);
	return (0);
}

int	check_right(t_token *paren, char **s)
{
	while (paren)
	{
		skip_redirs(&paren);
		if (paren && (paren->type == R_PAREN || is_connector(paren)))
			break ;
		else if (paren && paren->type == SPC)
			paren = paren->next;
		else if (paren)
		{
			*s = paren->lexeme;
			return (UNEX_TOK);
		}
	}
	return (0);
}

int	check_paren(t_token **tok, char **s);

int	check_mid(t_token **tok, char **s)
{
	int	err;

	err = 0;
	while (*tok)
	{
		if ((*tok)->type == L_PAREN)
			err = check_paren(tok, s);
		else if ((*tok)->type == OR || (*tok)->type == AND
			|| (*tok)->type == PIPE)
		{
			err = check_connector(tok);
			*tok = (*tok)->next;
		}
		else if ((*tok)->type == R_PAREN)
			break ;
		else
			*tok = (*tok)->next;
		if (err != 0)
			return (err);
	}
	return (0);
}

int	check_paren(t_token **tok, char **s)
{
	t_token	*end;
	int		err;

	err = check_left(*tok);
	if (err != 0)
		return (err);
	skip(tok, L_PAREN);
	skip(tok, SPC);
	if (*tok && (*tok)->type == R_PAREN)
		return (UNEX_RPAR);
	else if (!*tok)
		return (UNEX_NL);
	err = check_mid(tok, s);
	if (err != 0)
		return (err);
	end = *tok;
	err = check_for_closing_paren(&end);
	if (err != 0)
		return (err);
	*tok = end->next;
	err = check_right(*tok, s);
	if (err != 0)
		return (err);
	*s = NULL;
	return (0);
}

void	print_syntax_error(int err, char *str)
{
	if (err == UNCLOSED)
		print_error("minishell: unexpected EOF while looking for matching: ",
			")");
	else if (err == UNEX_PIPE)
		print_error("minishell: syntax error near unexpected token: ", "|");
	else if (err == UNEX_OR)
		print_error("minishell: syntax error near unexpected token: ", "||");
	else if (err == UNEX_AND)
		print_error("minishell: syntax error near unexpected token: ", "&&");
	else if (err == UNEX_RPAR)
		print_error("minishell: syntax error near unexpected token: ", ")");
	else if (err == UNEX_NL)
		print_error("minishell: syntax error near unexpected token: ",
			"newline");
	else if (err == UNEX_PAREN)
		print_error("minishell: syntax error near unexpected token: ", "(");
	else if (err == UNEX_TOK)
		print_error("minishell: syntax error near unexpected token: ", str);
}

void	check_syntax(t_token **tokens)
{
	t_token	*ptr;
	int		err;
	char	*str;

	ptr = *tokens;
	err = 0;
	while (ptr)
	{
		if (ptr->type == L_PAREN)
			err = check_paren(&ptr, &str);
		else if (ptr->type == OR || ptr->type == AND || ptr->type == PIPE)
			err = check_connector(&ptr);
		if (err != 0 || (ptr && ptr->type == R_PAREN))
		{
			if (err != 0)
				print_syntax_error(err, str);
			else
				print_syntax_error(UNEX_RPAR, NULL);
			*tokens = NULL;
			return ;
		}
		if (ptr)
			ptr = ptr->next;
	}
}
