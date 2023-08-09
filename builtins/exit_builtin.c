/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/13 20:36:31 by mmisskin          #+#    #+#             */
/*   Updated: 2023/08/09 15:29:08 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	print_exit_error(t_env **env, char *s1, char *s2)
{
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	if (s1)
	{
		ft_putstr_fd(s1, STDERR_FILENO);
		ft_putstr_fd(s2, STDERR_FILENO);
		ft_putstr_fd("\n", STDERR_FILENO);
		clean_env_list(env);
		clean_all();
		return ;
	}
	ft_putstr_fd(s2, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

int	is_number(char *s)
{
	int	i;
	int	sign;

	i = 0;
	sign = 1;
	if (s[0] == '-' || s[0] == '+')
	{
		if (s[0] == '-')
			sign = -1;
		s++;
	}
	while (s[i])
	{
		if (!ft_isdigit(s[i]))
			return (0);
		i++;
	}
	if (ft_strlen(s) > 19
		|| (sign == -1 && ft_atoull(s) > 9223372036854775808ULL)
		|| (sign == 1 && ft_atoull(s) > 9223372036854775807ULL))
		return (-1);
	return (1);
}

void	_exit_(t_env **env, char **cmd)
{
	int	j;
	int	status;

	j = 1;
	status = 0;
	ft_putstr_fd("exit\n", STDERR_FILENO);
	while (cmd[j])
	{
		if (j == 1 && is_number(cmd[j]) <= 0)
		{
			print_exit_error(env, cmd[j], ": numeric argument required");
			exit(255);
		}
		else if (j == 1)
			status = ft_atoi(cmd[j]);
		else if (j != 1)
		{
			print_exit_error(env, NULL, "too many arguments");
			return ;
		}
		j++;
	}
	exit(status);
}
