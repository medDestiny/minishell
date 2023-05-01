/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/06 12:50:54 by mmisskin          #+#    #+#             */
/*   Updated: 2023/05/01 22:51:58 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"shell.h"

char	*ft_strdup(const char *s1)
{
	size_t	size;
	int		i;
	char	*dest;

	i = 0;
	size = ft_strlen(s1) + 1;
	dest = (char *)malloc(size * sizeof(char));
	if (!dest)
		return (NULL);
	while (s1[i])
	{
		dest[i] = s1[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}
