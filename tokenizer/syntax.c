/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/06 20:07:24 by mmisskin          #+#    #+#             */
/*   Updated: 2023/06/06 20:10:59 by mmisskin         ###   ########.fr       */
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

//int	check_left(t_token *tok)
//{
//	t_token	*err;
//
//	if (tok)
//		err = tok->next;
//	while (tok)
//	{
//		while (tok->prev && tok->prev->type != SPACE && tok->type != OR && tok->type != AND && tok->type != PIPE)
//			tok = tok->prev;
//		if (tok && (tok->type == HEREDOC || tok->type == APPEND || tok->type == REDIR_IN || tok->type == REDIR_OUT))
//			tok = tok->prev;
//		else if (tok && tok->type == SPACE)
//			tok = tok->prev;
//		else if (tok && (tok->type == OR || tok->type == AND || tok->type == PIPE || tok->type == L_PAREN))
//			return (1);
//		else
//			break ;
//	}
//	if (!tok)
//		return (1);
//	print_error("minishell: syntax error near unexpected token: ", err->lexeme);
//	return (0);
//}
//
//int	check_right(t_token *tok)
//{
//	t_token	*err;
//
//	if (tok && tok->type == SPACE)
//		tok = tok->next;
//	err = tok;
//	while (tok)
//	{
//		if (tok->type == PIPE || tok->type == OR || tok->type == AND || tok->type == R_PAREN)
//			return (1);
//		else if (tok->type == REDIR_IN || tok->type == REDIR_OUT || tok->type == HEREDOC || tok->type == APPEND)
//		{
//			while (tok && tok->type != SPACE)
//				tok = tok->next;
//			err = tok->next;
//		}
//		tok = tok->next;
//	}
//	if (err)
//		print_error("minishell: syntax error near unexpected token: ", err->lexeme);
//	else
//		return (1);
//	return (0);
//}
//
//int	check_closing_paren(t_token **tok)
//{
//	int	err;
//
//	skip(tok, L_PAREN);
//	skip(tok, SPACE);
//	if (*tok && (*tok)->type == L_PAREN)
//	{
//		err = check_closing_paren(tok);
//		if (err == 1 || err == 3)
//			return (err);
//	}
//	else if (*tok && (*tok)->type == R_PAREN)
//	{
//		print_error("minishell: syntax error near unexpected token: )", NULL);
//		return (1);
//	}
//	else if (!*tok)
//		return (3);
//	while (*tok && (*tok)->type != R_PAREN)
//		*tok = (*tok)->next;
//	if (!*tok)
//		return (2);
//	else
//		*tok = (*tok)->next;
//	return (0);
//}
//
//int	check_paren(t_token **tok, int *open)
//{
//	int		closed;
//	t_token	*tmp;
//
//	tmp = *tok;
//	if ((*tok)->type == L_PAREN)
//	{
//		*open = 1;
//		if (!check_left((*tok)->prev))
//			return (1);
//		closed = check_closing_paren(&tmp);
//		if (closed == 2 || closed == 3)
//		{
//			if (closed == 2)
//				print_error("minishell: unexpected EOF while looking for matching: ", ")");
//			else
//				print_error("minishell: syntax error near unexpected token: ", "newline");
//			return (1);
//		}
//		else if (closed == 1)
//			return (1);
//		*tok = (*tok)->next; // Skip L_PAREN
//		//if (!check_left((*tok)->prev))
//		//	return (1);
//		while (*tok && (*tok)->type != R_PAREN)
//		{
//			if (*tok && (*tok)->type == L_PAREN)
//			{
//				if (check_paren(tok, open) != 0)
//					return (1);
//				continue ;
//			}
//			*tok = (*tok)->next;
//		}
//		if (*tok && (*tok)->type == R_PAREN)
//			*tok = (*tok)->next;
//	}
//	//if ((*tok)->type == R_PAREN)
//	//{
//	//	if (!*open)
//	//	{
//	//		print_error("minishell: syntax error near unexpected token `)'", NULL);
//	//		return (1);
//	//	}
//	//	else
//	//	{
//	//		*open = 0;
//	//		if (!check_right((*tok)->next))
//	//			return (1);
//	//		*tok = (*tok)->next; // Skip R_PAREN
//	//	}
//	//}
//	return (0);
//}
//
//void	check_syntax(t_token **tokens)
//{
//	t_token	*ptr;
//	int		open_paren;
//	int		err;
//	//char	*err;
//
//	err = 0;
//	ptr = *tokens;
//	open_paren = 0;
//	while (ptr)
//	{
//		if (ptr->type == L_PAREN)
//			err = check_paren(&ptr, &open_paren);
//		if (ptr && ptr->type == R_PAREN)
//		{
//			print_error("minishell: syntax error near unexpected token: )", NULL);
//			err = 1;
//		}
//		//if (ptr->type == L_PAREN || ptr->type == R_PAREN)
//		//	err = check_paren(&ptr, &open_paren);
//		//else if (ptr->type == PIPE || ptr->type == OR || ptr->type == AND)
//		//{
//		//	ptr = ptr->next;
//		//	if (ptr->type == SPACE)
//		//		ptr = ptr->next;
//		//	if (ptr->type == OR || ptr->type == AND || ptr->type == PIPE)
//		//	{
//		//		printf("minishell: syntax error near unexpected token: %s\n", ptr->lexeme);
//		//		*tokens = NULL;
//		//		return ;
//		//	}
//		//}
//		//else
//		if (ptr)
//			ptr = ptr->next;
//		if (err != 0)
//		{
//			*tokens = NULL;
//			return ;
//		}
//	}
//}

void	check_syntax(t_token **tokens)
{

}
