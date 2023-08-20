/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmisskin <mmisskin@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/10 11:04:12 by mmisskin          #+#    #+#             */
/*   Updated: 2023/08/19 21:51:09 by mmisskin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	sig_handler(int signum)
{
	if (signum == SIGINT)
	{
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
		g_exit.status = 1;
	}
}

void	hdoc_handler(int signum)
{
	if (signum == SIGINT)
		exit(EXIT_FAILURE);
}

void	heredoc_signals(void)
{
	struct sigaction sig;

	sig.sa_handler = hdoc_handler; 
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	if (sigaction(SIGINT, &sig, NULL) == -1)
		perror(NULL);
	sig.sa_handler = SIG_IGN;
	if (sigaction(SIGQUIT, &sig, NULL) == -1)
		perror(NULL);
}

void	default_signals(void)
{
	struct sigaction sig;

	sig.sa_handler = SIG_DFL;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	if (sigaction(SIGINT, &sig, NULL) == -1)
		perror(NULL);
	if (sigaction(SIGQUIT, &sig, NULL) == -1)
		perror(NULL);
}

void	ignore_signals(void)
{
	struct sigaction sig;

	sig.sa_handler = SIG_IGN;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	if (sigaction(SIGINT, &sig, NULL) == -1)
		perror(NULL);
	if (sigaction(SIGQUIT, &sig, NULL) == -1)
		perror(NULL);
}

void	signal_interrupter(void)
{
	struct sigaction sig;

	sig.sa_handler = sig_handler;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	if (sigaction(SIGINT, &sig, NULL) == -1)
		perror(NULL);
	sig.sa_handler = SIG_IGN;
	if (sigaction(SIGQUIT, &sig, NULL) == -1)
		perror(NULL);
}
