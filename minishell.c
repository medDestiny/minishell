/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/19 17:06:13 by mmisskin          #+#    #+#             */
/*   Updated: 2023/08/08 13:49:52 by hlaadiou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_type(t_node_type type)
{
	if (type == WORD)
		printf("WORD  ");
	else if (type == PIPE)
		printf("PIPE  ");
	else if (type == RD_IN)
		printf("REDIR_IN  ");
	else if (type == RD_IN_EXP)
		printf("REDIR_IN_EXP  ");
	else if (type == RD_OUT)
		printf("REDIR_OUT  ");
	else if (type == RD_OUT_EXP)
		printf("REDIR_OUT_EXP  ");
	else if (type == OR)
		printf("OR  ");
	else if (type == AND)
		printf("AND  ");
	else if (type == HDOC)
		printf("HEREDOC  ");
	else if (type == HDOC_EXP)
		printf("HEREDOC_EXP  ");
	else if (type == APPEND)
		printf("APPEND  ");
	else if (type == APPEND_EXP)
		printf("APPEND_EXP  ");
	else if (type == R_PAREN)
		printf("R_PAREN  ");
	else if (type == L_PAREN)
		printf("L_PAREN  ");
	else if (type == SPC)
		printf("SPC  ");
	else if (type == D_QUOTE)
		printf("D_QUOTE  ");
	else if (type == S_QUOTE)
		printf("S_QUOTE  ");
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
		ptr = root->cmd.redir;
		indent(lvl);
		while (ptr)
		{
			printf("|%s| ", ptr->lexeme);
			ptr = ptr->next;
		}
		printf("\n");
		ptr = root->cmd.sub_redir;
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

void	print_tokens(t_tree *root, t_env *env)
{
	t_token	*p;

	(void)env;
	if (root)
	{
		p = root->cmd.redir;
		//p = tkn_split(root->cmd.list);
		//p = list_expand(root->cmd.list, env);
		while (p)
		{
			print_type(p->type);
			printf("|%s| ", p->lexeme);
			p = p->next;
		}
		printf("\n");
	}
	return ;
}

void	minishell_loop(t_env *envp)
{
	char		*line;
	char		*cmdline;
	char		*shell;
	t_token		*tokens;
	t_tree		*root;
	//char		**v;

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
		print_tokens(root, envp);
		// if (root)
		//		print_tree(root, 0);
		//	v = ft_split(cmdline, ' ');
		//	if (!v || !*v)
		//		continue ;
		//	if (!ft_strcmp(v[0], "export"))
		//		_export(v, &envp, 1);
		//	else if (!ft_strcmp(v[0], "pwd"))
		//		_pwd(v, 1);
		//	else if (!ft_strcmp(v[0], "cd"))
		//		_cd(v, envp, 1);
		//	else if (!ft_strcmp(v[0], "env"))
		//		_env(envp, v, 1);
		//	else if (!ft_strcmp(v[0], "unset"))
		//		_unset(&envp, v);
		//	else if (!ft_strcmp(v[0], "echo"))
		//		_echo(v, 1);
		//	else if (!ft_strcmp(v[0], "exit"))
		//		_exit_(&envp, v);
		//else
		//	exec_cmd(root, envp);
		//clean_vec(v);
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
	minishell_loop(envp);
	clean_env_list(&envp);
}
