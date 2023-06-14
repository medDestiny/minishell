/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/19 17:06:13 by mmisskin          #+#    #+#             */
/*   Updated: 2023/06/14 18:16:02 by mmisskin         ###   ########.fr       */
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
	else if (type == RD_IN)
		printf("REDIR_IN\t");
	else if (type == RD_OUT)
		printf("REDIR_OUT\t");
	else if (type == OR)
		printf("OR\t");
	else if (type == AND)
		printf("AND\t");
	else if (type == HDOC)
		printf("HEREDOC\t");
	else if (type == APPEND)
		printf("APPEND\t");
	else if (type == R_PAREN)
		printf("R_PAREN\t");
	else if (type == L_PAREN)
		printf("L_PAREN\t");
	else if (type == SPC)
		printf("SPC\t");
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

void	minishell_loop(t_env *envp)
{
	char		*line;
	char		*cmdline;
	char		*shell;
	t_token		*tokens;
	t_tree		*root;
//	char		*v[] = {"cd", "..", NULL};
	char		**v;

	line = NULL;
	g_gc = NULL;
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
		root = parser(&tokens);
		if (root)
			print_tree(root, 0);
		v = ft_split(cmdline, ' ');
		//if (!ft_strcmp(v[0], "export"))
		//	_export(v, &envp, 1);
		//else if (!ft_strcmp(v[0], "pwd"))
		//	_pwd(v, 1);
		//else if (!ft_strcmp(v[0], "cd"))
		//	_cd(v, envp, 1);
		//else if (!ft_strcmp(v[0], "env"))
		//	_env(envp, v, 1);
		//else if (!ft_strcmp(v[0], "unset"))
		//	_unset(&envp, v);
		//else if (!ft_strcmp(v[0], "echo"))
		//	_echo(v, 1);
		//else if (!ft_strcmp(v[0], "exit"))
		//	_exit_(&envp, v);
		clean_vec(v);
		free(cmdline);
		clean_all(&g_gc);
	}
}

int	main(int ac, char **av, char **env)
{
	t_env		*envp;

	if (ac != 1)
		return (1);
	envp = env_dup(av[0], env);
	if (!envp)
		return (1);
	atexit(leak);
	minishell_loop(envp);
	clean_env_list(&envp);
}
