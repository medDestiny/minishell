/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_builtin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 15:45:14 by mmisskin          #+#    #+#             */
/*   Updated: 2023/06/10 17:51:36 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	env(t_env *env, int fd)
{
	while (env)
	{
		if (env->name && env->value)
		{
			ft_putstr_fd(env->name, fd);
			ft_putstr_fd("=", fd);
			ft_putstr_fd(env->value, fd);
			ft_putstr_fd("\n", fd);
		}
		env = env->next;
	}
}
