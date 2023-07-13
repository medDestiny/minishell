/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 08:48:38 by mmisskin          #+#    #+#             */
/*   Updated: 2023/06/17 15:32:44 by mmisskin         ###   ########.fr       */
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

# include<stdlib.h>
# include<stdbool.h>
# include<unistd.h>
# include<fcntl.h>
# include<stdio.h>
# include<readline/readline.h>
# include<readline/history.h>
# include<term.h>
# include<string.h>
# include<sys/errno.h>

typedef enum s_node_type
{
	PIPE,
	RD_IN_EXP,
	RD_IN,
	RD_OUT_EXP,
	RD_OUT,
	HDOC_EXP,
	HDOC,
	APPEND_EXP,
	APPEND,
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
	int		global_in;
	int		global_out;
	t_token	*list;
	t_token	*in;
	t_token	*out;
}	t_cmd;

typedef struct s_tree	t_tree;

typedef struct s_node
{
	t_tree	*lchild;
	t_tree	*rchild;
}	t_node;

typedef enum e_type
{
	T_AND,
	T_OR,
	T_PIPE,
	T_CMD,
}	t_type;

typedef struct s_tree
{
	t_type		type;
	t_node		node;
	t_cmd		cmd;
}	t_tree;

t_garb					*g_gc;

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
char		**ft_split(char const *s, char c);
char		*ft_substr(char const *s, unsigned int start, size_t len);
int			ft_isalnum(int c);
int			ft_isalpha(int c);
int			ft_isdigit(int c);

//	Environment
t_env		*env_dup(char *prog_name, char **env);
t_env		*get_env_node(t_env *env, char *name);
int			env_add(t_env **env, char *name, char *value);
char		*get_env_value(t_env *env, char *name);
void		update_env_value(t_env **env, char *name, char *new_val,
				int append);

//	Memory management
void		clean_env_list(t_env **envp);
void		clean_vec(char **vec);

//	Garbage Collector
void		*ft_malloc(size_t size, t_garb **gc);
void		clean_all(t_garb **gc);

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
size_t	ft_atoull(const char *str);

//	Builtin functions
void		_env(t_env *env, char **arg, int fd);
void		_pwd(char **cmd, int fd);
void		_cd(char **cmd, t_env *env, int fd);
void		_export(char **cmd, t_env **env, int fd);
void		_unset(t_env **env, char **cmd);
void		_echo(char **cmd, int fd);
void		_exit_(t_env **env, char **cmd);

#endif
