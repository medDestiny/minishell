/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaadiou <hlaadiou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 10:04:39 by hlaadiou          #+#    #+#             */
/*   Updated: 2023/08/17 23:38:11 by hlaadiou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	cmd_not_found(char *cmd)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(cmd, STDERR_FILENO);
	ft_putstr_fd(": command not found\n", STDERR_FILENO);
}

void	cmd_error(char *lexeme)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(lexeme, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	ft_putstr_fd(strerror(errno), STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

int	is_a_directory(char *lexeme)
{
	struct stat	fstatus;

	if (stat(lexeme, &fstatus) == 0)
	{
		if (S_ISDIR(fstatus.st_mode))
		{
			ft_putstr_fd("minishell: ", STDERR_FILENO);
			ft_putstr_fd(lexeme, STDERR_FILENO);
			ft_putstr_fd(": is a directory\n", STDERR_FILENO);
			return (1);
		}
	}
	return (0);
}

char	*cmd_prefix(char *lexeme, t_env *envp)
{
	char	**paths;
	char	*cmd_abs;
	char	*path;
	size_t	i;

	i = 0;
	cmd_abs = NULL;
	paths = ft_split(get_env_value(envp, "PATH"), ":");
	while (paths && paths[i])
	{
		path = ft_strjoin(paths[i], "/");
		cmd_abs = ft_strjoin(path, lexeme);
		free(path);
		if (access(cmd_abs, F_OK) == -1)
		{
			free(cmd_abs);
			i++;
		}
		else
			break ;
	}
	if (!paths || !paths[i])
	{
		cmd_not_found(lexeme);
		exit(127);
	}
	if (access(cmd_abs, X_OK) == -1)
	{
		cmd_error(lexeme);
		exit(126);
	}
	return (clean_vec(paths), cmd_abs);
}

char	*check_path(char *lexeme, t_env *envp)
{
	if (lexeme && (!ft_strncmp(lexeme, "/", 1) || !ft_strncmp(lexeme, "./", 2) \
		|| !ft_strncmp(lexeme, "../", 3)))
	{
		if (access(lexeme, F_OK) == 0)
		{
			if (access(lexeme, X_OK) == -1)
			{
				cmd_error(lexeme);
				exit(126);
			}
		}
		else
		{
			cmd_error(lexeme);
			exit(127);
		}
		if (is_a_directory(lexeme))
			exit(126);
		return (ft_strdup(lexeme));
	}
	return (cmd_prefix(lexeme, envp));
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
	vec = (char **)ft_malloc(words * sizeof(char *));
	if (!vec)
		return (NULL);
	vec[i++] = check_path(list->lexeme, envp);
	if (!vec[0])
		return (NULL);
	list = list->next;
	while (list && i < words)
	{
		vec[i++] = list->lexeme;
		list = list->next;
	}
	vec[i] = NULL;
	return (vec);
}

size_t	lst_size(t_env *env)
{
	size_t	size;

	size = 0;
	while (env)
	{
		size++;
		env = env->next;
	}
	return (size);
}

size_t	env_size(t_env *env)
{
	size_t	size;

	size = 0;
	while (env)
	{
		if (env->value && !env->hide)
			size++;
		env = env->next;
	}
	return (size);
}

char	**build_env_vec(t_env *env)
{
	size_t	i;
	size_t	words;
	char	**vec;
	char	*joined;

	i = 0;
	words = env_size(env) + 1;
	vec = (char **)malloc(words * sizeof(char *));
	if (!vec)
		return (NULL);
	while (env && i < words)
	{
		if (env && env->value && !env->hide)
		{
			joined = ft_strjoin(env->name, "=");
			vec[i] = ft_strjoin(joined, env->value);
			free(joined);
			i++;
		}
		env = env->next;
	}
	vec[i] = NULL;
	return (vec);
}

void	_error(char *file)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	perror(file);
}

int	is_append(t_node_type type)
{
	if (type == APPEND_WD || type == APPEND_DQ || type == APPEND_SQ)
		return (1);
	return (0);
}

int	is_rd_in(t_node_type type)
{
	if (type == RD_IN_WD || type == RD_IN_DQ || type == RD_IN_SQ)
		return (1);
	return (0);
}

int	is_rd_out(t_node_type type)
{
	if (type == RD_OUT_WD || type == RD_OUT_DQ || type == RD_OUT_SQ)
		return (1);
	return (0);
}

int	is_hdoc(t_node_type type)
{
	if (type == HDOC || type == HDOC_EXP)
		return (1);
	return (0);
}

int	*open_files(t_token *redir_list)
{
	t_token	*redir;
	t_token	*tmpredir;
	int		*fildes;
	int		tmpfd;

	fildes = (int *)malloc(2 * sizeof(int));
	if (!fildes)
		return (NULL);
	redir = redir_list;
	tmpredir = NULL;
	fildes[0] = STDIN_FILENO;
	fildes[1] = STDOUT_FILENO;
	while (redir)
	{
		if (is_hdoc(redir->type))
		{
			if (fildes[0] != STDIN_FILENO)
				close(fildes[0]);
			fildes[0] = open_heredoc(redir);
		}
		redir = redir->next;
	}
	redir = redir_list;
	while (redir)
	{
		if (is_append(redir->type))
		{
			tmpfd = open(redir->lexeme, O_CREAT | O_WRONLY | O_APPEND, 0644);
			if (tmpfd == -1)
				return (_error(redir->lexeme), NULL);
			if (fildes[1] != STDOUT_FILENO)
				close(fildes[1]);
			fildes[1] = tmpfd;
		}
		else if (is_rd_out(redir->type))
		{
			tmpfd = open(redir->lexeme, O_CREAT | O_WRONLY | O_TRUNC, 0644);
			if (tmpfd == -1)
				return (_error(redir->lexeme), NULL);
			if (fildes[1] != STDOUT_FILENO)
				close(fildes[1]);
			fildes[1] = tmpfd;
		}
		else if (is_rd_in(redir->type))
		{
			tmpfd = open(redir->lexeme, O_RDONLY);
			if (tmpfd == -1)
				return (_error(redir->lexeme), NULL);
			close(tmpfd);
			tmpredir = redir;
		}
		redir = redir->next;
	}
	redir = lst_last(redir_list);
	while (redir)
	{
		if (is_hdoc(redir->type))
			break ;
		else if (redir == tmpredir)
		{
			tmpfd = open(redir->lexeme, O_RDONLY);
			if (tmpfd == -1)
				return (_error(redir->lexeme), NULL);
			if (fildes[0] != STDIN_FILENO)
				close(fildes[0]);
			fildes[0] = tmpfd;
		}
		redir = redir->prev;
	}
	return (fildes);
}

void	set_fildes(int *files)
{
	if (!files)
		exit(EXIT_FAILURE);
	if (files[0] != STDIN_FILENO)
	{
		dup2(files[0], STDIN_FILENO);
		close(files[0]);
	}
	if (files[1] != STDOUT_FILENO)
	{
		dup2(files[1], STDOUT_FILENO);
		close(files[1]);
	}
	free(files);
}

void	execute_child(t_tree *node, t_env *env)
{
	int		*files;
	char	**cmd_vec;
	char	**env_vec;

	default_signals();
	files = open_files(node->cmd.redir);
	if (!node->cmd.list)
		exit(0);
	cmd_vec = build_vec(node->cmd.list, env);
	env_vec = build_env_vec(env);
	set_fildes(files);
	execve(cmd_vec[0], cmd_vec, env_vec);
	cmd_not_found(node->cmd.list->lexeme);
	exit(EXIT_FAILURE);
}

int	is_builtin(t_token *cmd)
{
	if (cmd && cmd->lexeme)
	{
		if (!ft_strcmp(cmd->lexeme, "cd") \
			|| !ft_strcmp(cmd->lexeme, "echo") \
			|| !ft_strcmp(cmd->lexeme, "pwd") \
			|| !ft_strcmp(cmd->lexeme, "export") \
			|| !ft_strcmp(cmd->lexeme, "unset") \
			|| !ft_strcmp(cmd->lexeme, "env") \
			|| !ft_strcmp(cmd->lexeme, "exit"))
			return (1);
	}
	return (0);
}

size_t	tkn_lst_size(t_token *lst)
{
	size_t	size;

	size = 0;
	while (lst)
	{
		size++;
		lst = lst->next;
	}
	return (size);
}

char	**build_builtin_vec(t_token *cmd)
{
	size_t	i;
	size_t	size;
	char	**vec;

	i = 0;
	size = tkn_lst_size(cmd) + 1;
	vec = (char **)malloc(size * sizeof(char *));
	if (!vec)
		return (NULL);
	while (i < size - 1)
	{
		vec[i] = ft_strdup(cmd->lexeme);
		i++;
		cmd = cmd->next;
	}
	vec[i] = NULL;
	return (vec);
}

void	execute_builtin(char **cmd, t_env **env, int *files, int flag)
{
	if (!ft_strcmp(cmd[0], "cd"))
		_cd(cmd, *env, files[1]);
	else if (!ft_strcmp(cmd[0], "echo"))
		_echo(cmd, files[1]);
	else if (!ft_strcmp(cmd[0], "pwd"))
		_pwd(*env, cmd, files[1]);
	else if (!ft_strcmp(cmd[0], "export"))
		_export(cmd, env, files[1]);
	else if (!ft_strcmp(cmd[0], "unset"))
		_unset(env, cmd);
	else if (!ft_strcmp(cmd[0], "env"))
		_env(*env, cmd, files[1]);
	else if (!ft_strcmp(cmd[0], "exit"))
		_exit_(env, cmd, flag);
}

void	exec_builtins(t_tree *node, t_env **env, int flag)
{
	int		*files;
	char	**cmd_vec;

	if (flag == 1)
		default_signals();
	files = open_files(node->cmd.redir);
	if (!files)
		return ;
	cmd_vec = build_builtin_vec(node->cmd.list);
	if (cmd_vec && cmd_vec[0])
		execute_builtin(cmd_vec, env, files, flag);
	if (files[0] != STDIN_FILENO)
		close(files[0]);
	if (files[1] != STDOUT_FILENO)
		close(files[1]);
	free(files);
	clean_vec(cmd_vec);
	if (flag == 1)
		exit(g_exit.status);
}

int	check_for_builtins(t_tree *node, t_env **env)
{
	pid_t	pid;

	if (!is_builtin(node->cmd.list))
		return (0);
	if (node->type == S_CMD)
	{
		pid = fork();
		if (pid == -1)
		{
			_error("fork failure");
			return (1);
		}
		if (pid == 0)
			exec_builtins(node, env, 1);
		else
		{
			ignore_signals();
			waitpid(pid, &g_exit.status, 0);
			signal_interrupter();
			if (WIFEXITED(g_exit.status))
				g_exit.status = WEXITSTATUS(g_exit.status);
			else if (WIFSIGNALED(g_exit.status))
				g_exit.status = 128 + WTERMSIG(g_exit.status);
		}
	}
	else
		exec_builtins(node, env, 0);
	return (1);
}

int	execute_command(t_tree *node, t_env **env)
{
	pid_t	pid;

	if (check_for_builtins(node, env))
		return (0);
	pid = fork();
	if (pid == -1)
	{
		_error("fork failure");
		return (1);
	}
	if (pid == 0)
		execute_child(node, *env);
	else
	{
		ignore_signals();
		waitpid(pid, &g_exit.status, 0);
		signal_interrupter();
	}
	if (WIFEXITED(g_exit.status))
		g_exit.status = WEXITSTATUS(g_exit.status);
	else if (WIFSIGNALED(g_exit.status))
		g_exit.status = 128 + WTERMSIG(g_exit.status);
	return (0);
}

int	exec_cmd(t_tree *node, t_env **env)
{
	int	ret_exp;

	if (!node)
		return (1);
	ret_exp = node_expand(&node->cmd, *env);
	if (ret_exp == ALLOCERR)
		return (ALLOCERR);
	else if (ret_exp == 0)
		if (execute_command(node, env) != 0)
			return (1);
	return (0);
}

int	exec_or(t_tree *node, t_env **env)
{
	int	ret;

	ret = executor(node->node.lchild, env);
	if (ret != 0)
		return (ret);
	if (g_exit.status != 0)
	{
		ret = executor(node->node.rchild, env);
		if (ret != 0)
			return (ret);
	}
	return (0);
}

int	exec_and(t_tree *node, t_env **env)
{
	int	ret;

	ret = executor(node->node.lchild, env);
	if (ret != 0)
		return (ret);
	if (g_exit.status == 0)
	{
		ret = executor(node->node.rchild, env);
		if (ret != 0)
			return (ret);
	}
	return (0);
}

int	is_subshell(t_type type)
{
	if (type == S_PIPE \
		|| type == S_OR \
		|| type == S_AND \
		|| type == S_CMD)
		return (1);
	return (0);
}

void	exec_subshell(t_tree *subsh, t_env **env)
{
	if (subsh->type == S_OR)
		exec_or(subsh, env);
	else if (subsh->type == S_AND)
		exec_and(subsh, env);
	else if (subsh->type == S_PIPE)
	{
		//exec_pipe
	}
	else
		exec_cmd(subsh, env);
	exit(g_exit.status);
}

void	subshell(t_tree *subsh, t_env **env)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		_error("fork failure");
		return ;
	}
	if (pid == 0)
		exec_subshell(subsh, env);
	else
	{
		ignore_signals();
		waitpid(pid, &g_exit.status, 0);
		signal_interrupter();
	}
	if (WIFEXITED(g_exit.status))
		g_exit.status = WEXITSTATUS(g_exit.status);
	else if (WIFSIGNALED(g_exit.status))
		g_exit.status = 128 + WTERMSIG(g_exit.status);
}

int	executor(t_tree *root, t_env **env)
{
	int	err;

	err = 0;
	if (!root)
		return (1);
	if (is_subshell(root->type))
		subshell(root, env);
	else if (root->type == T_OR)
		err = exec_or(root, env);
	else if (root->type == T_AND)
		err = exec_and(root, env);
	else if (root->type == PIPE)
	{
		//exec_pipe
	}
	else
		err = exec_cmd(root, env);
	if (err == ALLOCERR)
		return (err);
	return (0);
}
