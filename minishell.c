/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/19 17:06:13 by mmisskin          #+#    #+#             */
/*   Updated: 2023/08/20 22:10:39 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//void	print_type(t_node_type type)
//{
//	if (type == WORD)
//		printf("WORD  ");
//	else if (type == PIPE)
//		printf("PIPE  ");
//	else if (type == RD_IN_SQ)
//		printf("REDIR_IN_SQ  ");
//	else if (type == RD_IN_DQ)
//		printf("REDIR_IN_DQ  ");
//	else if (type == RD_IN_WD)
//		printf("REDIR_IN_WD  ");
//	else if (type == RD_OUT_SQ)
//		printf("REDIR_OUT_SQ  ");
//	else if (type == RD_OUT_DQ)
//		printf("REDIR_OUT_DQ  ");
//	else if (type == RD_OUT_WD)
//		printf("REDIR_OUT_WD  ");
//	else if (type == OR)
//		printf("OR  ");
//	else if (type == AND)
//		printf("AND  ");
//	else if (type == HDOC)
//		printf("HEREDOC  ");
//	else if (type == HDOC_EXP)
//		printf("HEREDOC_EXP  ");
//	else if (type == APPEND_SQ)
//		printf("APPEND_SQ  ");
//	else if (type == APPEND_DQ)
//		printf("APPEND_EXP_DQ  ");
//	else if (type == APPEND_WD)
//		printf("APPEND_EXP_WD  ");
//	else if (type == R_PAREN)
//		printf("R_PAREN  ");
//	else if (type == L_PAREN)
//		printf("L_PAREN  ");
//	else if (type == SPC)
//		printf("SPC  ");
//	else if (type == D_QUOTE)
//		printf("D_QUOTE  ");
//	else if (type == S_QUOTE)
//		printf("S_QUOTE  ");
//}
//
//void	print_vec(char **vec)
//{
//	int	i;
//
//	i = 0;
//	while (vec[i])
//	{
//		printf("[%d] - %s\n", i, vec[i]);
//		i++;
//	}
//}
//
//void	indent(int lvl)
//{
//	while (lvl--)
//		printf("\t");
//}
//
//void	print_tree(t_tree *root, int lvl)
//{
//	t_token	*ptr;
//
//	if (root->type == T_CMD)
//	{
//		ptr = root->cmd.list;
//		indent(lvl);
//		printf("CMD\n");
//		indent(lvl);
//		while (ptr)
//		{
//			printf("|%s| ", ptr->lexeme);
//			ptr = ptr->next;
//		}
//		printf("\n");
//		ptr = root->cmd.redir;
//		indent(lvl);
//		while (ptr)
//		{
//			printf("|%s| ", ptr->lexeme);
//			ptr = ptr->next;
//		}
//		printf("\n");
//		ptr = root->cmd.sub_redir;
//		indent(lvl);
//		while (ptr)
//		{
//			printf("|%s| ", ptr->lexeme);
//			ptr = ptr->next;
//		}
//		printf("\n");
//	}
//	else if (root->type == T_PIPE || root->type == T_OR || root->type == T_AND)
//	{
//		indent(lvl + 1);
//		if (root->type == T_PIPE)
//			printf("PIPE\n");
//		else if (root->type == T_OR)
//			printf("OR\n");
//		else
//			printf("AND\n");
//		print_tree(root->node.rchild, lvl + 2);
//		print_tree(root->node.lchild, lvl);
//	}
//}
//
//void	print_tokens(t_tree *root, t_env *env)
//{
//	t_token	*p;
//
//	(void)env;
//	if (root)
//	{
//		p = NULL;
//	//	p = redir_expand(root->cmd.redir, env);
//	//	while (p)
//	//	{
//	//		print_type(p->type);
//	//		printf("|%s| ", p->lexeme);
//	//		p = p->next;
//	//	}
//		p = root->cmd.list;
//		while (p)
//		{
//			print_type(p->type);
//			printf("|%s| ", p->lexeme);
//			p = p->next;
//		}
//		printf("\n");
//		p = root->cmd.redir;
//		while (p)
//		{
//			print_type(p->type);
//			printf("|%s| ", p->lexeme);
//			p = p->next;
//		}
//		printf("\n");
//		p = root->cmd.sub_redir;
//		while (p)
//		{
//			print_type(p->type);
//			printf("|%s| ", p->lexeme);
//			p = p->next;
//		}
//		printf("\n");
//	}
//	return ;
//}

void	clean_exit(t_env **env, char *line)
{
	clean_env_list(env);
	clean_all();
	free(line);
	rl_on_new_line();
	rl_redisplay();
	printf("exit\n");
	exit(g_exit.status);
}

char	*trim_line(char *line)
{
	char	*cmdline;

	cmdline = ft_strtrim(line, BLANK);
	add_history(line);
	free(line);
	return (cmdline);
}

t_tree	*evaluate_cmd(char *cmdline)
{
	t_tree	*root;
	t_token	*tokens;

	tokens = lexer(cmdline);
	root = parser(&tokens);
	return (root);
}

void	minishell_loop(t_env *envp)
{
	t_tree		*root;
	char		*line;
	char		*cmdline;
	char		*shell;

	while (1)
	{
		shell = prompt(envp);
		line = readline(shell);
		free(shell);
		if (!line)
			clean_exit(&envp, line);
		else if (!*line)
		{
			free(line);
			continue ;
		}
		cmdline = trim_line(line);
		root = evaluate_cmd(cmdline);
		if (open_heredocs(root, envp) == 0)
			executor(root, &envp);
		free(cmdline);
		clean_all();
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
	signal_interrupter();
	minishell_loop(envp);
	clean_env_list(&envp);
}
