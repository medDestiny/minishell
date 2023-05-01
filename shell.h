/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 12:40:54 by mmisskin          #+#    #+#             */
/*   Updated: 2023/05/01 23:59:15 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHELL_H
# define SHELL_H

# include<stdlib.h>
# include<stdbool.h>
# include<unistd.h>
# include<fcntl.h>
# include<stdio.h>
# include<readline/readline.h>
# include<readline/history.h>
# include<ncursesw/curses.h>
# include<term.h>

void	clean_vec(char **vec);
char	*ft_strdup(const char *s1);
size_t	ft_strlen(const char *str);
char	*ft_strjoin(char const *s1, char const *s2);
int		ft_strncmp(const char *s1, const char *s2, size_t n);

#endif
