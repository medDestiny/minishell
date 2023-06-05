/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/19 17:06:13 by mmisskin          #+#    #+#             */
/*   Updated: 2023/06/05 09:02:07 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	leak()
{
	system("leaks msh");
}

void	print_type(t_node_type type)
{
	if (type == WORD)
		printf("WORD\t");
	else if (type == PIPE)
		printf("PIPE\t");
	else if (type == REDIR_IN)
		printf("REDIR_IN\t");
	else if (type == REDIR_OUT)
		printf("REDIR_OUT\t");
	else if (type == OR)
		printf("OR\t");
	else if (type == AND)
		printf("AND\t");
	else if (type == HEREDOC)
		printf("HEREDOC\t");
	else if (type == APPEND)
		printf("APPEND\t");
	else if (type == R_PAREN)
		printf("R_PAREN\t");
	else if (type == L_PAREN)
		printf("L_PAREN\t");
	else if (type == SPACE)
		printf("SPACE\t");
	else if (type == D_QUOTE)
		printf("D_QUOTE\t");
	else if (type == S_QUOTE)
		printf("S_QUOTE\t");
}

void	print_vec(char **vec)
{
	int	i;

	i = 0;
	while (vec[i])
	{
		printf("[%d] - %s\n", i, vec[i]);
		i++;
	}
}

void	indent(int lvl)
{
	while (lvl--)
		printf("\t");
}

void	print_tree(t_tree *root, int lvl)
{
	t_token	*ptr;

	if (root->type == T_CMD)
	{
		ptr = root->cmd.list;
		indent(lvl);
		printf("CMD\n");
		indent(lvl);
		while (ptr)
		{
			printf("|%s| ", ptr->lexeme);
			ptr = ptr->next;
		}
		printf("\n");
	}
	else if (root->type == T_PIPE || root->type == T_OR || root->type == T_AND)
	{
		indent(lvl + 1);
		if (root->type == T_PIPE)
			printf("PIPE\n");
		else if (root->type == T_OR)
			printf("OR\n");
		else
			printf("AND\n");
		print_tree(root->node.rchild, lvl + 2);
		print_tree(root->node.lchild, lvl);
	}
}

int	main(int ac, char **av, char **env)
{
	t_env	*envp;
	char	*line;
	char	*cmdline;
	char	*shell;
	t_token	*tokens;
//	t_token	*next;
	t_tree	*root;

	(void)av;
	if (ac != 1)
		return (1);
	envp = env_dup(env);
	if (!envp)
		return (1);
	line = NULL;
	//atexit(leak);
	while (1)
	{
		shell = prompt(envp);
		line = readline(shell);
		free(shell);
		if (!line)
			break ;
		cmdline = ft_strtrim(line, " \t");
		add_history(line);
		free(line);
		tokens = lexer(cmdline);
		if (!tokens)
			continue ;
		root = parser(&tokens);
		if (root)
			print_tree(root, 0);
		//if (root)
		//	print_vec(root->cmd.cmd);
		//printf("your command was: \x1B[31m\"\e[0m%s\x1B[31m\"\e[0m\n", cmdline);
		//tokens = root->cmd.list;
		//while (tokens)
		//{
		//	next = tokens->next;
		//	print_type(tokens->type);
		//	printf("\x1B[31m|\e[0m%s\x1B[31m|\e[0m\n", tokens->lexeme);
		//	free(tokens->lexeme);
		//	free(tokens);
		//	tokens = next;
		//}
		free(cmdline);
	}
	clean_env_list(envp);
}
