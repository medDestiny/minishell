/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaadiou <hlaadiou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 10:04:39 by hlaadiou          #+#    #+#             */
/*   Updated: 2023/07/14 13:08:47 by hlaadiou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*cmd_prefix(char *lexeme, t_env *envp)
{
	char	**paths;
	char	*cmd_abs;
	char	*path;
	size_t	i;

	i = 0;
	cmd_abs = NULL;
	paths = ft_split(get_env_value(envp, "PATH"), ':');
	while(paths && paths[i])
	{
		path = ft_strjoin(paths[i], "/");
		cmd_abs = ft_strjoin(path, lexeme);
		free(path);
		if (access(cmd_abs, F_OK | X_OK) == -1)
		{
			free(cmd_abs);
			i++;
		}
		else
			break;
	}
	clean_vec(paths);
	return (cmd_abs);
}

size_t	count_words(t_token *list)
{
	size_t	words;

	words = 0;
	while (list)
	{
		if (list->type == WORD)
			words++;
		list = list->next;
	}
	return (words);
}

char	**build_vec(t_token *list, t_env *envp)
{
	size_t	i;
	size_t	words;
	char	**vec;

	i = 0;
	vec = NULL;
	words = count_words(list) + 1;
	vec = (char**)ft_malloc(words * sizeof(char*), &g_gc);
	while (list && i < words)
	{
		if (list->type == WORD)
		{
			if (!i)
				vec[i++] = cmd_prefix(list->lexeme, envp);
			else
				vec[i++] = list->lexeme;
		}
		list = list->next;
	}
	vec[i] = NULL;
	return (vec);
}

int	exec_cmd(t_tree *node, t_env *envp)
{
	char	**cmd;
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (1);
	if (pid == 0)
	{
		if (node && node->type == T_CMD)
		{
			cmd = build_vec(node->cmd.list, envp);
			if (execve(cmd[0], cmd, NULL) == -1)
			{
				perror(NULL);
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			printf("U should handle more cases u fucking loser ;)\n");
			exit(EXIT_FAILURE);
		}
	}
	waitpid(-1, NULL, 0);
	return (1);
}
