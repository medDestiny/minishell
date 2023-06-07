/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 08:48:38 by mmisskin          #+#    #+#             */
/*   Updated: 2023/06/07 17:41:39 by mmisskin         ###   ########.fr       */
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

typedef enum s_node_type
{
	PIPE,
	REDIR_IN,
	REDIR_OUT,
	HEREDOC,
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

 /////////////
typedef struct s_redir
{
	t_node_type		type;
	char			*file;
	struct s_redir	*next;
}	t_redir;

typedef struct s_cmd
{
	int		subshell;
	int		global_in;
	int		global_out;
	t_token	*list;
	t_token	*in;
	t_token	*out;
}	t_cmd;

typedef struct u_tree t_tree;

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

typedef struct u_tree
{
	t_type		type;
	t_node		node;
	t_cmd		cmd;
}	t_tree;
 ///////////////

//	Libft
size_t	ft_strlen(const char *str);
char	*ft_strdup(const char *s1);
int		ft_strcmp(char *str1, char *str2);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
char	*ft_strjoin(char const *s1, char const *s2);
char	*ft_strtrim(char const *s1, char const *set);
char	*ft_strchr(const char *s, int c);
char	*ft_strrchr(const char *s, int c);
size_t	ft_strlcpy(char *dst, const char *src, size_t dstsize);
void	ft_putstr_fd(char *s, int fd);

//	Environment
t_env	*env_dup(char **env);
char	*get_env_value(t_env *env, char *name);

//	Memory management
void	clean_env_list(t_env *envp);

char	*prompt(t_env *env);
t_token	*lexer(char *cmdline);

int	token_list_add(t_token **list, t_node_type type, char *content, size_t size);

void	check_syntax(t_token **tokens);
t_tree	*parser(t_token **tokens);
int	parse_group(t_tree **root, t_token **tokens);
t_node_type	peek(t_token *tokens);
void	skip_redirs(t_token **tokens);
void	skip(t_token **tokens, t_node_type type);
int	is_connector(t_token *tok);
void	print_syntax_error(int err, char *str);

#endif
