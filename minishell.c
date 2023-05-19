/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/19 17:06:13 by mmisskin          #+#    #+#             */
/*   Updated: 2023/05/19 18:20:13 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(int ac, char **av, char **env)
{
	t_env	*envp;
	char	*line;
	char	*shell;

	(void)av;
	if (ac != 1)
		return (1);
	envp = env_dup(env);
	line = NULL;
	while (1)
	{
		shell = prompt(envp);
		line = readline(shell);
		free(shell);
		if (!line)
			break ;
		free(line);
	}
}
