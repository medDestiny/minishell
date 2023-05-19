/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 08:48:38 by mmisskin          #+#    #+#             */
/*   Updated: 2023/05/19 18:20:44 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include<stdlib.h>
# include<stdbool.h>
# include<unistd.h>
# include<fcntl.h>
# include<stdio.h>
# include<readline/readline.h>
# include<readline/history.h>
# include<term.h>

typedef struct s_env
{
	char			*name;
	char			*value;
	struct s_env	*next;
}	t_env;

//	Libft
size_t	ft_strlen(const char *str);
char	*ft_strdup(const char *s1);
int		ft_strcmp(char *str1, char *str2);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
char	*ft_strjoin(char const *s1, char const *s2);

//	Environment
t_env	*env_dup(char **env);
char	*get_env_value(t_env *env, char *name);

char	*prompt(t_env *env);

#endif
