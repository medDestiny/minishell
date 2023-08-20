/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_expander.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaadiou <hlaadiou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 17:31:10 by hlaadiou          #+#    #+#             */
/*   Updated: 2023/08/20 04:42:23 by hlaadiou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ambiguous_redir(char *str)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(str, STDERR_FILENO);
	ft_putstr_fd(": ambiguous redirect\n", STDERR_FILENO);
	return ;
}

t_node_type	hdoc_pick_type(t_token *lst)
{
	t_node_type	inherited;

	inherited = lst->type;
	if (lst && inherited == HDOC_EXP)
	{
		lst = lst->next;
		while (lst && lst->type != SPC && !is_redir_in(lst->type) \
				&& !is_redir_out(lst->type))
		{
			if (lst->type == D_QUOTE || lst->type == S_QUOTE)
				return (HDOC);
			lst = lst->next;
		}
	}
	return (inherited);
}

void	join_filename(t_token **redir, char **newfile)
{
	char	*tmp;
	int		toggle;

	toggle = 1;
	while (toggle \
			|| (*redir && (*redir)->type != SPC && !is_redir_in((*redir)->type) \
			&& !is_redir_out((*redir)->type)))
	{
		toggle = 0;
		tmp = *newfile;
		*newfile = ft_strjoin(tmp, (*redir)->lexeme);
		free(tmp);
		(*redir) = (*redir)->next;
	}
}

t_token	*redir_join(t_token *redir)
{
	t_node_type	inherited;
	t_token		*joined;
	char		*newfile;

	joined = NULL;
	while (redir)
	{
		newfile = NULL;
		inherited = hdoc_pick_type(redir);
		join_filename(&redir, &newfile);
		if (newfile)
		{
			if (token_list_add(&joined, inherited, newfile, \
			ft_strlen(newfile)) != 0)
			{
				free(newfile);
				return (NULL);
			}
		}
		if (redir && redir->type == SPC)
			redir = redir->next;
		free(newfile);
	}
	return (joined);
}

int	expandable_tkn(t_token *lst)
{
	if (!*lst->lexeme)
		return (0);
	while (lst && !is_redir_in(lst->type) && !is_redir_out(lst->type))
		lst = lst->prev;
	if (lst && (lst->type == HDOC || lst->type == HDOC_EXP))
		return (0);
	return (1);
}

int	wild_redir_match(t_token **lst, t_entry *matches, char *dir, \
		t_node_type type)
{
	char	*wildlex;

	if ((is_redir_in(type) || is_redir_out(type)) && matches->next)
	{
		g_exit.status = AMBGRDIR;
		return (1);
	}
	while (matches)
	{
		if (dir)
			wildlex = ft_strjoin(dir, matches->name);
		else
			wildlex = matches->name;
		if (!wildlex || \
		token_list_add(lst, type, wildlex, ft_strlen(wildlex)) != 0)
			return (1);
		if (dir)
			free(wildlex);
		matches = matches->next;
	}
	return (0);
}

int	wild_redir_exp(t_token **newlst, t_token *tkn, int **flags, int *pos)
{
	t_entry	*matches;
	char	*pattern;
	char	*dir;

	dir = NULL;
	pattern = NULL;
	matches = NULL;
	if (ft_strchr(tkn->lexeme, '*') || ft_strchr(tkn->lexeme, '?'))
	{
		extract_dir_pattern(&dir, &pattern, tkn);
		matches = dir_pattern_check(dir, pattern, flags[(*pos)++]);
	}
	if (matches)
	{
		if (wild_redir_match(newlst, matches, dir, tkn->type) == 1)
			return (free(dir), free(pattern), clean_list(&matches), 1);
	}
	else
		if (token_list_add(newlst, tkn->type, tkn->lexeme, \
				ft_strlen(tkn->lexeme)) != 0)
			return (free(dir), free(pattern), 1);
	free(pattern);
	free(dir);
	clean_list(&matches);
	return (0);
}

t_token	*wild_redirlst_exp(t_token *rdirlst, int **flags)
{
	t_token	*wild_rdirlst;
	int		pos;

	pos = 0;
	wild_rdirlst = NULL;
	if (!flags)
		return (NULL);
	while (rdirlst)
	{
		if (wild_redir_exp(&wild_rdirlst, rdirlst, flags, &pos) == 1)
		{
			if (g_exit.status == AMBGRDIR)
				ambiguous_redir(rdirlst->lexeme);
			return (NULL);
		}
		rdirlst = rdirlst->next;
	}
	return (wild_rdirlst);
}

int	expandable_rdir_tilde(t_token *lst)
{
	if (!lst)
		return (0);
	if ((lst->type == WORD || lst->type == RD_IN_WD || lst->type == RD_OUT_WD \
		|| lst->type == APPEND_WD) \
		&& *lst->lexeme == '~' \
		&& (!*(lst->lexeme + 1)) \
		&& (!lst->next || (lst->next && lst->next->type == SPC)) \
		&& (!lst->prev || (lst->prev && lst->prev->type == SPC)))
		return (1);
	else if ((lst->type == WORD || lst->type == RD_IN_WD \
			|| lst->type == RD_OUT_WD \
			|| lst->type == APPEND_WD) \
			&& *lst->lexeme == '~' \
			&& *(lst->lexeme + 1) && *(lst->lexeme + 1) == '/' \
			&& (!lst->prev || (lst->prev && lst->prev->type == SPC)))
		return (1);
	else
		return (0);
}

void	expand_rdir_tilde(t_token *lst, t_env *env)
{
	char	*home;
	char	*newlex;

	home = get_home(env);
	if (!home)
		return ;
	while (lst)
	{
		newlex = NULL;
		if (expandable_rdir_tilde(lst))
		{
			if (!*(lst->lexeme + 1))
				newlex = ft_strdup(home);
			else if (*(lst->lexeme + 1) && *(lst->lexeme + 1) == '/')
				newlex = ft_strjoin(home, lst->lexeme + 1);
			if (newlex)
			{
				lst->lexeme = newlex;
				garbage_list_add(&g_exit.gc, newlex);
			}
		}
		lst = lst->next;
	}
	free(home);
}

void	assign_rd_in(t_token *tkn)
{
	if (tkn->next && tkn->next->type == WORD)
		tkn->next->type = RD_IN_WD;
	else if (tkn->next && tkn->next->type == D_QUOTE)
		tkn->next->type = RD_IN_DQ;
	else if (tkn->next && tkn->next->type == S_QUOTE)
		tkn->next->type = RD_IN_SQ;
}

void	assign_rd_out(t_token *tkn)
{
	if (tkn->next && tkn->next->type == WORD)
		tkn->next->type = RD_OUT_WD;
	else if (tkn->next && tkn->next->type == D_QUOTE)
		tkn->next->type = RD_OUT_DQ;
	else if (tkn->next && tkn->next->type == S_QUOTE)
		tkn->next->type = RD_OUT_SQ;
}

void	assign_append(t_token *tkn)
{
	if (tkn->next && tkn->next->type == WORD)
		tkn->next->type = APPEND_WD;
	else if (tkn->next && tkn->next->type == D_QUOTE)
		tkn->next->type = APPEND_DQ;
	else if (tkn->next && tkn->next->type == S_QUOTE)
		tkn->next->type = APPEND_SQ;
}

void	choose_type(t_token *tkn)
{
	if (tkn && (tkn->type == RD_IN_WD || tkn->type == RD_IN_DQ \
		|| tkn->type == RD_IN_SQ))
		assign_rd_in(tkn);
	else if (tkn && (tkn->type == RD_OUT_WD || tkn->type == RD_OUT_DQ \
		|| tkn->type == RD_OUT_SQ))
		assign_rd_out(tkn);
	else if (tkn && (tkn->type == APPEND_WD || tkn->type == APPEND_DQ \
		|| tkn->type == APPEND_SQ))
		assign_append(tkn);
}

int	check_ambiguity(t_token *tkn, t_env *env)
{
	t_token	*original;
	char	*error;
	char	*errtmp;

	original = tkn;
	error = ft_strdup(tkn->lexeme);
	tkn = tkn->next;
	while (tkn)
	{
		if (tkn->type == WORD && tkn->lexeme && *tkn->lexeme == '$' \
			&& *(tkn->lexeme + 1) && !get_env_value(env, tkn->lexeme + 1))
		{
			errtmp = error;
			error = ft_strjoin(errtmp, tkn->lexeme);
			free(errtmp);
			tkn = tkn->next;
		}
		else if (tkn->type != SPC \
			&& !is_redir_in(tkn->type) && !is_redir_out(tkn->type))
			return (free(error), 0);
		else
			break ;
	}
	ambiguous_redir(error);
	return (free(error), 1);
}

char	*get_splitted_redir_var(t_env *env, char *var)
{
	char	**splitted;
	char	*value;
	int		len;

	len = 0;
	value = get_env_value(env, var + 1);
	splitted = ft_split(value, BLANK);
	while (splitted && splitted[len])
		len++;
	clean_vec(splitted);
	if (len > 1)
	{
		ambiguous_redir(var);
		g_exit.status = AMBGRDIR;
		return (NULL);
	}
	return (value);
}

void	truncate_dollar(t_token *tkn, char **var)
{
	if ((tkn->type == WORD || tkn->type == RD_IN_WD || tkn->type == RD_OUT_WD \
			|| tkn->type == APPEND_WD) && tkn->next && \
			(tkn->next->type == D_QUOTE || tkn->next->type == S_QUOTE))
	{
		if (tkn->type == RD_IN_WD || tkn->type == RD_OUT_WD \
			|| tkn->type == APPEND_WD)
			choose_type(tkn);
		*var = NULL;
	}
}

char	*grep_value(t_token	*tkn, t_env *env)
{
	char	*var;

	var = tkn->lexeme;
	if (var && *var == '$')
	{
		if (*(var + 1))
		{
			var = get_splitted_redir_var(env, var);
			if (!var && g_exit.status == AMBGRDIR)
				return (NULL);
			if (!var && (tkn->type == RD_OUT_DQ || tkn->type == RD_IN_DQ \
				|| tkn->type == APPEND_DQ || tkn->type == D_QUOTE))
				var = "";
			else if (!var && \
					(is_redir_in(tkn->type) || is_redir_out(tkn->type)))
			{
				if (check_ambiguity(tkn, env))
					return (g_exit.status = AMBGRDIR, NULL);
				choose_type(tkn);
			}
		}
		else
			truncate_dollar(tkn, &var);
	}
	return (var);
}

char	*hdoc_expand(t_token *tkn)
{
	char	*var;

	var = tkn->lexeme;
	if ((tkn->type == HDOC_EXP || tkn->type == WORD) && tkn->next \
		&& (tkn->next->type == D_QUOTE || tkn->next->type == S_QUOTE))
	{
		if (var && *var == '$' && !*(var + 1))
		{
			if (tkn->type == HDOC_EXP)
				tkn->next->type = HDOC;
			else
				tkn->next->type = hdoc_pick_type(tkn->next);
			var = NULL;
		}
	}
	return (var);
}

int	expandable_redir(t_token *lst)
{
	if (lst->type == RD_IN_WD || lst->type == RD_OUT_WD \
	|| lst->type == APPEND_WD \
	|| (lst->type == WORD && expandable_tkn(lst)) \
	|| lst->type == RD_IN_DQ || lst->type == RD_OUT_DQ \
	|| lst->type == APPEND_DQ \
	|| (lst->type == D_QUOTE && expandable_tkn(lst)))
		return (1);
	return (0);
}

int	redir_update(t_token **redirlst, t_token *lst, t_env *env)
{
	char		*var;

	while (lst && lst->lexeme)
	{
		var = lst->lexeme;
		if (expandable_redir(lst))
			var = grep_value(lst, env);
		else if (lst->type == HDOC || lst->type == HDOC_EXP \
				|| !expandable_tkn(lst))
			var = hdoc_expand(lst);
		if (!var && g_exit.status == AMBGRDIR)
			return (1);
		if (var && \
			token_list_add(redirlst, lst->type, var, ft_strlen(var)) != 0)
			return (1);
		lst = lst->next;
	}
	return (0);
}

void	set_redir_exit_status(t_token *redir)
{
	while (redir)
	{
		if (redir->type == RD_IN_WD || redir->type == RD_IN_DQ \
		|| redir->type == RD_OUT_WD || redir->type == RD_OUT_DQ \
		|| redir->type == APPEND_WD || redir->type == APPEND_DQ \
		|| (redir->type == WORD && expandable_tkn(redir)) \
		|| (redir->type == D_QUOTE && expandable_tkn(redir)))
		{
			if (redir->lexeme && !ft_strcmp(redir->lexeme, "$?"))
			{
				redir->lexeme = ft_itoa(g_exit.status);
				if (redir->lexeme)
					garbage_list_add(&g_exit.gc, redir->lexeme);
			}
		}
		redir = redir->next;
	}
}

int	expand_redir_vars(t_token **newredir, t_token *lst, t_env *env)
{
	t_token	*splitted;

	splitted = redirlst_split(lst);
	set_redir_exit_status(splitted);
	if (redir_update(newredir, splitted, env))
		return (1);
	return (0);
}

t_token	*redir_expand(t_token *redir, t_env *env)
{
	t_token	*newredir;
	int		*flagtab;
	int		**flagvec;

	newredir = NULL;
	flagvec = NULL;
	if (env)
	{
		if (expand_redir_vars(&newredir, redir, env) == 1)
			return (NULL);
		expand_rdir_tilde(newredir, env);
	}
	flagtab = create_wildflags(newredir);
	newredir = redir_join(newredir);
	if (flagtab)
	{
		flagvec = create_wildvec(flagtab, newredir);
		free(flagtab);
		newredir = wild_redirlst_exp(newredir, flagvec);
		clean_intvec(flagvec);
	}
	return (newredir);
}
