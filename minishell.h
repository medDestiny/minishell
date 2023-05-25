/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 08:48:38 by mmisskin          #+#    #+#             */
/*   Updated: 2023/05/25 00:24:53 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# define BLANK " \t"
# define SEPARATOR " \t|&\"\'<>()"
# define HDOC_SEP "|&<>()"

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
	SPACE,
	S_QUOTE,
	D_QUOTE,
	WORD
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

//	Environment
t_env	*env_dup(char **env);
char	*get_env_value(t_env *env, char *name);

//	Memory management
void	clean_env_list(t_env *envp);

char	*prompt(t_env *env);
t_token	*lexer(char *cmdline);

#endif
