/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_expander.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaadiou <hlaadiou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 17:31:10 by hlaadiou          #+#    #+#             */
/*   Updated: 2023/08/15 00:45:18 by hlaadiou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	ambiguous_redir(char *str)
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

// Update the token if it is of type WORD or D_QUOTE
// into a new expanded token added to the 'newtknlst'.
//int	redir_update(t_token **newlst, t_token *lst, t_env *env)
//{
//	char	*var;
//	t_token	*sub;
//
//	sub = redirlst_split(lst);
//	if (!sub)
//	{
//		if (g_exit.status == ALLOCERR)
//			return (1);
//		return (0);
//	}
//	while (sub && sub->lexeme)
//	{
//		var = sub->lexeme;
//		//Case of a variable that will be eliminated but inside quotes.
//		if (*var == '$' && *(var + 1) && \
//		(lst->type == D_QUOTE || lst->type == RD_IN_DQ || lst->type == RD_OUT_DQ || \
//		lst->type == APPEND_DQ) && !get_env_value(env, var + 1))
//			var = "";
//		//Case of a variable that has a value or no value
//		else if (*var == '$' && *(var + 1))
//		{
//			var = get_env_value(env, (var + 1));
//			if (!var && sub->next && sub->next->type != SPC && !is_redir_in(sub->next->type)\
//				&& !is_redir_out(sub->next->type))
//				sub->next->type = sub->type;
//			else if (!var)
//			{
//				if (check_ambiguous(&sub, env))
//					return (1);
//			}
//		}
//		//case of a dollar sign that will be truncated
//		else if (*var == '$' && lst->next \
//		&& (lst->next->type == D_QUOTE || lst->next->type == S_QUOTE))
//			var = NULL;
//		if (var)
//			if (token_list_add(newlst, sub->type, var, ft_strlen(var)) != 0)
//				return (1);
//		sub = sub->next;
//	}
//	return (0);
//}

//int	expandable_dq(t_token *lst)
//{
//	if (!*lst->lexeme)
//		return (0);
//	while (lst && !is_redir_in(lst->type) && !is_redir_out(lst->type))
//		lst = lst->prev;
//	if (lst && (lst->type == HDOC || lst->type == HDOC_EXP))
//		return (0);
//	return (1);
//}

static void	fill_flagtab(int *tab, int *flags, int len, int *ind)
{
	int	i;

	i = 0;
	while (i < len)
	{
		tab[i] = flags[*ind];
		i++;
		(*ind)++;
	}
}

static void	create_flagtabs(int **vec, int *flags, t_token *lst)
{
	int	len;
	int	ind;
	int	i;
	int	j;

	j = -1;
	ind = 0;
	while (lst)
	{
		i = -1;
		len = 0;
		while (lst->lexeme[++i])
			if (in_set(lst->lexeme[i], "*?"))
				len++;
		if (len)
		{
			vec[++j] = (int *)malloc(sizeof(int) * len);
			if (!vec[j])
				return ;
			fill_flagtab(vec[j], flags, len, &ind);
		}
		lst = lst->next;
	}
}

static int	**create_wildvec(int *flags, t_token *lst)
{
	t_flags	vars;
	int		i;

	vars.tkn = lst;
	vars.flagvec = NULL;
	vars.arrs = 0;
	while (vars.tkn)
	{
		i = -1;
		while (vars.tkn->lexeme[++i])
			if (in_set(vars.tkn->lexeme[i], "*?") && ++vars.arrs)
				break ;
		vars.tkn = vars.tkn->next;
	}
	if (vars.arrs)
	{
		vars.flagvec = (int **)malloc(sizeof(int *) * (vars.arrs + 1));
		if (!vars.flagvec)
			return (NULL);
		create_flagtabs(vars.flagvec, flags, lst);
	}
	return (vars.flagvec[vars.arrs] = NULL, vars.flagvec);
}

static void	extract_dir_pattern(char **dir, char **pattern, t_token *tkn)
{
	char	*separator;
	int		dir_len;

	separator = ft_strrchr(tkn->lexeme, '/');
	if (!separator)
		*pattern = ft_strdup(tkn->lexeme);
	else
	{
		dir_len = ft_strlen(tkn->lexeme) - ft_strlen(separator + 1);
		*pattern = ft_substr(separator + 1, 0, ft_strlen(separator + 1));
		*dir = ft_substr(tkn->lexeme, 0, dir_len);
	}
	return ;
}

static int	wildlex_match(t_token **lst, t_entry *matches, char *dir, \
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

static int	wildtkn_expand(t_token **newlst, t_token *tkn, int **flags, int *pos)
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
		if (wildlex_match(newlst, matches, dir, tkn->type) == 1)
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

static t_token	*wildlst_expand(t_token *tknlst, int **flags)
{
	t_token	*wildtknlst;
	int		pos;

	pos = 0;
	wildtknlst = NULL;
	if (!flags)
		return (NULL);
	while (tknlst)
	{
		if (wildtkn_expand(&wildtknlst, tknlst, flags, &pos) == 1)
			return (NULL);
		tknlst = tknlst->next;
	}
	return (wildtknlst);
}

static char	*get_home(t_env *env)
{
	char	*home;

	home = get_env_value(env, "HOME");
	if (!home)
		home = getenv("HOME");
	if (home)
		return (ft_strdup(home));
	return (NULL);
}

static int	expandable_tilde(t_token *lst)
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

static void	expand_tilde(t_token *lst, t_env *env)
{
	char	*home;
	char	*newlex;

	home = get_home(env);
	if (!home)
		return ;
	while (lst)
	{
		newlex = NULL;
		if (expandable_tilde(lst))
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

void	choose_type(t_token *tkn)
{
	if (tkn && (tkn->type == RD_IN_WD || tkn->type == RD_IN_DQ \
		|| tkn->type == RD_IN_SQ))
	{
		if (tkn->next && tkn->next->type == WORD)
			tkn->next->type = RD_IN_WD;
		else if (tkn->next && tkn->next->type == D_QUOTE)
			tkn->next->type = RD_IN_DQ;
		else if (tkn->next && tkn->next->type == S_QUOTE)
			tkn->next->type = RD_IN_SQ;
	}
	else if (tkn && (tkn->type == RD_OUT_WD || tkn->type == RD_OUT_DQ \
		|| tkn->type == RD_OUT_SQ))
	{
		if (tkn->next && tkn->next->type == WORD)
			tkn->next->type = RD_OUT_WD;
		else if (tkn->next && tkn->next->type == D_QUOTE)
			tkn->next->type = RD_OUT_DQ;
		else if (tkn->next && tkn->next->type == S_QUOTE)
			tkn->next->type = RD_OUT_SQ;
	}
	else if (tkn && (tkn->type == APPEND_WD || tkn->type == APPEND_DQ \
		|| tkn->type == APPEND_SQ))
	{
		if (tkn->next && tkn->next->type == WORD)
			tkn->next->type = APPEND_WD;
		else if (tkn->next && tkn->next->type == D_QUOTE)
			tkn->next->type = APPEND_DQ;
		else if (tkn->next && tkn->next->type == S_QUOTE)
			tkn->next->type = APPEND_SQ;
	}
}

int	check_ambiguity(t_token *tkn, t_env *env)
{
	t_token	*original;
	char	*error;
	char	*errtmp;

	original = tkn;
	error = NULL;
	tkn = tkn->next;
	while (tkn)
	{
		errtmp = NULL;
		if (tkn->type == WORD && tkn->lexeme && *tkn->lexeme == '$' \
			&& *(tkn->lexeme + 1) && !get_env_value(env, tkn->lexeme + 1))
		{
			errtmp = ft_strjoin(error, tkn->lexeme);
			error = errtmp;
			free(errtmp);
			tkn = tkn->next;
		}
		else
			return (free(error), 0);
	}
	ambiguous_redir(original->lexeme);
	return (free(error), 1);
}

char	*grep_value(t_token	*tkn, t_env *env)
{
	char	*var;

	var = tkn->lexeme;
	if (tkn->lexeme && *tkn->lexeme == '$')
	{
		if (*(tkn->lexeme + 1))
		{
			var = get_env_value(env, tkn->lexeme + 1);
			if (!var && (tkn->type == RD_OUT_DQ || tkn->type == RD_IN_DQ \
				|| tkn->type == APPEND_DQ || tkn->type == D_QUOTE))
					var = "";
			else if (!var && (is_redir_in(tkn->type) || is_redir_out(tkn->type)))
			{
				if (check_ambiguity(tkn, env))
					return (g_exit.status = AMBGRDIR, NULL);
				choose_type(tkn);
			}
		}
		else if ((tkn->type == WORD || tkn->type == RD_IN_WD || tkn->type == RD_OUT_WD \
				|| tkn->type == APPEND_WD) && tkn->next && \
				(tkn->next->type == D_QUOTE || tkn->next->type == S_QUOTE))
		{
			if (tkn->type == RD_IN_WD || tkn->type == RD_OUT_WD || tkn->type == APPEND_WD)
				choose_type(tkn);
			var = NULL;
		}
	}
	return (var);
}

int	redir_update(t_token **redirlst, t_token *lst, t_env *env)
{
	char		*var;

	while (lst && lst->lexeme)
	{
		var = lst->lexeme;
		if (lst->type == RD_IN_WD || lst->type == RD_OUT_WD || lst->type == APPEND_WD \
		|| lst->type == WORD || lst->type == RD_IN_DQ || lst->type == RD_OUT_DQ \
		|| lst->type == APPEND_DQ || lst->type == D_QUOTE)
			var = grep_value(lst, env);
		if (!var && g_exit.status == AMBGRDIR)
			return (g_exit.status = 1, 1);
		if (var && token_list_add(redirlst, lst->type, var, ft_strlen(var)) != 0)
			return (1);
		lst = lst->next;
	}
	return (0);
}

int	expand_redir_vars(t_token **newredir, t_token *lst, t_env *env)
{
	t_token	*splitted;

	splitted = redirlst_split(lst);
	if (redir_update(newredir, splitted, env))
		return (1);
//	while (lst)
//	{
//		if (lst->type == WORD || lst->type == RD_IN_WD \
//		|| lst->type == RD_OUT_WD || lst->type == APPEND_WD \
//		|| ((lst->type == D_QUOTE || lst->type == RD_IN_DQ \
//		|| lst->type == RD_OUT_DQ || lst->type == APPEND_DQ) 
//		&& *lst->lexeme && expandable_dq(lst))) 
//		{
//			if (redir_update(newredir, lst, env) == 1)
//				return (1);
//		}
//		else if (lst->next && (lst->next->type == S_QUOTE \
//				|| (lst->next->type == D_QUOTE && !expandable_dq(lst->next))) \
//				&& *lst->lexeme == '$' && !*(lst->lexeme + 1))
//		{
//			lst->next->type = HDOC;
//			lst->lexeme = NULL;
//		}
//		else
//			if (token_list_add(newredir, lst->type, lst->lexeme, \
//					ft_strlen(lst->lexeme)) != 0)
//				return (1);
//		lst = lst->next;
//	}
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
		expand_tilde(newredir, env);
	}
	flagtab = create_wildflags(newredir);
	newredir = redir_join(newredir);
	if (flagtab)
	{
		flagvec = create_wildvec(flagtab, newredir);
		free(flagtab);
		newredir = wildlst_expand(newredir, flagvec);
		if (!newredir && g_exit.status == AMBGRDIR)
			ambiguous_redir(redir->lexeme);
		clean_intvec(flagvec);
	}
	return (newredir);
}
