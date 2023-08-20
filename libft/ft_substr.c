/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/08 09:29:52 by mmisskin          #+#    #+#             */
/*   Updated: 2023/07/27 10:50:22 by hlaadiou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	i;
	size_t	j;
	size_t	l;
	char	*sub;

	i = 0;
	j = 0;
	if (!s)
		return (0);
	l = ft_strlen(s);
	if (len <= l)
		sub = (char *)malloc((len + 1) * sizeof(char));
	else
		sub = (char *)malloc((l - start + 1) * sizeof(char));
	if (!sub)
		return (0);
	while (s[start] && start < l && s[start + j] && j < len)
		sub[i++] = s[start + j++];
	sub[i] = '\0';
	return (sub);
}
