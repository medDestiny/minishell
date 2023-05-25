/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 09:57:58 by mmisskin          #+#    #+#             */
/*   Updated: 2023/05/22 12:06:32 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"../minishell.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	int		index;
	int		len1;
	char	*dest;

	if (!s1 || !s2)
		return (0);
	index = 0;
	len1 = ft_strlen(s1);
	dest = (char *)malloc((len1 + ft_strlen(s2) + 1) * sizeof(char));
	if (!dest)
		return (NULL);
	while (s1[index])
	{
		dest[index] = s1[index];
		index++;
	}
	index = 0;
	while (s2[index])
	{
		dest[len1 + index] = s2[index];
		index++;
	}
	dest[len1 + index] = '\0';
	return (dest);
}
