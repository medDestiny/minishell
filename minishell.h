/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaadiou <hlaadiou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 08:48:38 by mmisskin          #+#    #+#             */
/*   Updated: 2023/08/17 21:58:32 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# define BLANK " \t"
# define SEPARATOR " \t|&\"\'<>()"
# define HDOC_SEP "|&<>()"

# define UNCLOSED 1
# define UNEX_PAREN 3
# define UNEX_PIPE 4
# define UNEX_OR 5
# define UNEX_AND 6
# define UNEX_NL 7
# define UNEX_RPAR 8
# define UNEX_TOK 9

# define ALLOCERR 1337
# define AMBGRDIR 4242
# define SYNTAXERR 258

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <fcntl.h>
# include <stddef.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/errno.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <signal.h>
# include <dirent.h>

typedef enum s_node_type
{
	PIPE,
	RD_IN_WD,
	RD_IN_SQ,
	RD_IN_DQ,
	RD_OUT_WD,
	RD_OUT_SQ,
	RD_OUT_DQ,
	APPEND_WD,
	APPEND_SQ,
	APPEND_DQ,
	HDOC_EXP,
	HDOC,
	AND,
	OR,
	R_PAREN,
	L_PAREN,
	SPC,
	S_QUOTE,
	D_QUOTE,
	WORD,
	END
}	t_node_type;

typedef struct s_env
{
	int				hide;
	char			*name;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_token
{
	t_node_type		type;
	char			*lexeme;
	struct s_token	*next;
	struct s_token	*prev;
}	t_token;

typedef struct s_garb
{
	void			*ptr;
	struct s_garb	*next;
}	t_garb;

typedef struct s_cmd
{
	int		subshell;
	t_token	*list;
	t_token	*redir;
	t_token	*sub_redir;
}	t_cmd;

typedef struct s_tree	t_tree;

typedef struct s_node
{
	t_tree	*lchild;
	t_tree	*rchild;
}	t_node;

typedef enum e_type
{
	S_AND,
	T_AND,
	S_OR,
	T_OR,
	S_PIPE,
	T_PIPE,
	T_CMD,
}	t_type;

typedef struct s_tree
{
	t_type		type;
	t_node		node;
	t_cmd		cmd;
}	t_tree;

typedef struct s_exit
{
	int		status;
	t_garb	*gc;
}	t_exit;

t_exit					g_exit;

typedef struct s_data
{
	int	i;
	int	j;
	int	str_i;
	int	pat_i;
	int	*flags;
	int	flags_i;
}	t_data;

typedef struct s_entry
{
	char			*name;
	struct s_entry	*next;
}	t_entry;

typedef struct s_flags
{
	t_token	*tkn;
	int		**flagvec;
	int		arrs;
}	t_flags;

/***************** Parsing ********************/

//	Libft
size_t		ft_strlen(const char *str);
char		*ft_strdup(const char *s1);
int			ft_strcmp(char *str1, char *str2);
int			ft_strncmp(const char *s1, const char *s2, size_t n);
char		*ft_strjoin(char const *s1, char const *s2);
char		*ft_strtrim(char const *s1, char const *set);
char		*ft_strchr(const char *s, int c);
char		*ft_strrchr(const char *s, int c);
size_t		ft_strlcpy(char *dst, const char *src, size_t dstsize);
void		ft_putstr_fd(char *s, int fd);
int			ft_atoi(const char *str);
char		*ft_itoa(int n);
char		**ft_split(char const *s, char *set);
char		*ft_substr(char const *s, unsigned int start, size_t len);
int			ft_isalnum(int c);
int			ft_isalpha(int c);
int			ft_isdigit(int c);

//	General purpose functions
t_token		*lst_last(t_token *tkn);

//	Environment
t_env		*env_dup(char *prog_name, char **env);
t_env		*get_env_node(t_env *env, char *name);
int			env_add(t_env **env, char *name, char *value, int hide);
char		*get_env_value(t_env *env, char *name);
void		update_env_value(t_env **env, char *name, char *new_val,
				int append);

//	Memory management
void		clean_env_list(t_env **envp);
void		clean_vec(char **vec);
void		clean_intvec(int **intvec);
void		clean_list(t_entry **list);

//	Garbage Collector
void		*ft_malloc(size_t size);
void		clean_all(void);
int			garbage_list_add(t_garb **gc, void *ptr);

//	Shell Prompt
char		*prompt(t_env *env);

//	Tokenizer functions
t_token		*lexer(char *cmdline);
int			token_list_add(t_token **list, t_node_type type, char *content,
				size_t size);
int			check_tokens(t_token **tokens, char *cmdline, char *limit);
int			check_word(t_token **tokens, char *cmdline, char *limit);
int			check_parenthesis(t_token **tokens, char *cmdline);
int			check_or(t_token **tokens, char *cmdline);
int			check_and(t_token **tokens, char *cmdline);
int			check_pipe(t_token **tokens, char *cmdline);
int			check_heredoc(t_token **tokens, char *cmdline);
int			check_append(t_token **tokens, char *cmdline);
int			check_infile(t_token **tokens, char *cmdline);
int			check_outfile(t_token **tokens, char *cmdline);
int			check_redirection(t_token **tokens, char *cmdline);
int			check_single_quotes(t_token **tokens, char *cmdline);
int			check_double_quotes(t_token **tokens, char *cmdline);
int			in_set(char c, char *set);

//	Syntax errors
void		check_syntax(t_token **tokens);
int			check_paren(t_token **tok, char **s);
void		print_error(char *s1, char *s2);
void		print_syntax_error(int err, char *str);

//	Recursive Descent Parser functions
t_tree		*parser(t_token **tokens);
int			parse_group(t_tree **root, t_token **tokens);
int			parse_condition(t_tree **root, t_token **tokens);
int			parse_pipeline(t_tree **root, t_token **tokens);
int			parse_command(t_tree **root, t_token **tok);

//	Give the type of the next token that is a connector or parens 
t_node_type	peek(t_token *tokens);

//	Check whether the token is an input redirection
int			is_redir_in(t_node_type type);

//	Check whether the token is an output redirection
int			is_redir_out(t_node_type type);

//	Skip the redirections in a pipeline
void		skip_redirs(t_token **tokens);

//	Skip a certain token
void		skip(t_token **tokens, t_node_type type);

//	Check if the token is a connector
int			is_connector(t_token *tok);

//	Ascii to unsigned long long
size_t		ft_atoull(const char *str);

//	Builtin functions
void		_env(t_env *env, char **arg, int fd);
void		_pwd(t_env *env, char **cmd, int fd);
void		_cd(char **cmd, t_env *env, int fd);
void		_export(char **cmd, t_env **env, int fd);
void		_unset(t_env **env, char **cmd);
void		_echo(char **cmd, int fd);
void		_exit_(t_env **env, char **cmd, int flag);

//	Signal handling
void		signal_interrupter(void);
void		ignore_signals(void);
void		default_signals(void);
void		heredoc_signals(void);

//	HEREDOC Function
int			open_heredoc(t_token *hdoc);

/***************** Wildcards ********************/

int			wildcard_match(char *str, char *pattern, int *flags);
t_entry		*dir_pattern_check(char *dir, char *pattern, int *flags);

/***************** Execution ********************/

t_token		*tkn_split(t_token *tkn);
t_token		*redirlst_split(t_token *redir);
t_token		*list_expand(t_token *tokens, t_env *env);
t_token		*redir_expand(t_token *redir, t_env *env);
int			*create_wildflags(t_token *tknlst);
int			node_expand(t_cmd *cmd_node, t_env *env);
void		sublist_insert(t_token *sublst, t_token **lleft, t_token **lright);
int			exec_cmd(t_tree *node, t_env **env);
int			executor(t_tree *root, t_env **env);

#endif
