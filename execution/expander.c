/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaadiou <hlaadiou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/15 08:13:22 by hlaadiou          #+#    #+#             */
/*   Updated: 2023/08/09 23:50:06 by hlaadiou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_token	*tkn_join(t_token *lst)
{
	t_token		*joined;
	char		*newlex;
	char		*tmp;

	joined = NULL;
	while (lst)
	{
		newlex = NULL;
		while (lst && lst->type != SPC)
		{
			tmp = newlex;
			newlex = ft_strjoin(tmp, lst->lexeme);
			free(tmp);
			lst = lst->next;
		}
		if (newlex)
			if (token_list_add(&joined, WORD, newlex, ft_strlen(newlex)) != 0)
				return (free(newlex), NULL);
		if (lst)
			lst = lst->next;
		free(newlex);
	}
	return (joined);
}

t_node_type	pick_type(t_token *lst)
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
		inherited = pick_type(redir);
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
int	tkn_update(t_token **newlst, t_token *lst, t_env *env)
{
	char	*var;
	t_token	*sub;

	sub = tkn_split(lst);
	if (!sub)
	{
		if (g_exit.status == ALLOCERR)
			return (1);
		return (0);
	}
	while (sub && sub->lexeme)
	{
		var = sub->lexeme;
		if (*var == '$' && *(var + 1))
			var = get_env_value(env, (var + 1));
		else if (*var == '$' && lst->next \
		&& (lst->next->type == D_QUOTE || lst->next->type == S_QUOTE))
			var = NULL;
		if (var)
			if (token_list_add(newlst, sub->type, var, ft_strlen(var)) != 0)
				return (1);
		sub = sub->next;
	}
	return (0);
}

int	expandable_dq(t_token *lst)
{
	if (!*lst->lexeme)
		return (0);
	while (lst && !is_redir_in(lst->type) && !is_redir_out(lst->type))
		lst = lst->prev;
	if (lst && (lst->type == HDOC || lst->type == HDOC_EXP))
		return (0);
	return (1);
}

int	do_not_wildexpand(t_token *tknlst)
{
	if (tknlst->type == D_QUOTE || tknlst->type == S_QUOTE \
	|| tknlst->type == RD_IN_DQ || tknlst->type == RD_IN_SQ \
	|| tknlst->type == RD_OUT_DQ || tknlst->type == RD_OUT_SQ \
	|| tknlst->type == APPEND_DQ || tknlst->type == APPEND_SQ \
	|| tknlst->type == HDOC_EXP || tknlst->type == HDOC \
	|| (tknlst->type == WORD && !expandable_dq(tknlst)))
		return (1);
	return (0);
}

void	fill_wildtab(int *flags, t_token *tknlst)
{
	int	i;
	int	j;

	j = 0;
	while (tknlst)
	{
		i = 0;
		while (tknlst->lexeme[i])
		{
			if (in_set(tknlst->lexeme[i], "*?"))
			{
				if (do_not_wildexpand(tknlst))
					flags[j++] = 0;
				else
					flags[j++] = 1;
			}
			i++;
		}
		tknlst = tknlst->next;
	}
	return ;
}

void	fill_flagtab(int *tab, int *flags, int len, int *ind)
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

void	create_flagtabs(int **vec, int *flags, t_token *lst)
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

int	**create_wildvec(int *flags, t_token *lst)
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

int	*create_wildflags(t_token *tknlst)
{
	t_token	*token;
	int		*flags;
	int		index;
	int		wildchars;

	flags = NULL;
	wildchars = 0;
	token = tknlst;
	while (token)
	{
		index = -1;
		while (token->lexeme[++index])
			if (in_set(token->lexeme[index], "*?"))
				wildchars++;
		token = token->next;
	}
	if (wildchars)
	{
		flags = (int *)malloc(wildchars * sizeof(int));
		if (!flags)
			return (NULL);
		fill_wildtab(flags, tknlst);
	}
	return (flags);
}

void	extract_dir_pattern(char **dir, char **pattern, t_token *tkn)
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

int	wildlex_match(t_token **lst, t_entry *matches, char *dir, \
		t_node_type type)
{
	char	*wildlex;

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

int	wildtkn_expand(t_token **newlst, t_token *tkn, int **flags, int *pos)
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

t_token	*wildlst_expand(t_token *tknlst, int **flags)
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

char	*get_home(t_env *env)
{
	char	*home;

	home = get_env_value(env, "HOME");
	if (!home)
		home = getenv("HOME");
	if (home)
		return (ft_strdup(home));
	return (NULL);
}

int	expandable_tilde(t_token *lst)
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

void	expand_tilde(t_token *lst, t_env *env)
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

int	expand_env_vars(t_token **newlst, t_token *lst, t_env *env)
{
	while (lst)
	{
		if (lst->type == WORD || (lst->type == D_QUOTE && *lst->lexeme) \
		|| lst->type == RD_IN_DQ || lst->type == RD_IN_WD \
		|| lst->type == RD_OUT_DQ || lst->type == RD_OUT_WD \
		|| lst->type == APPEND_DQ || lst->type == APPEND_WD)
		{
			if (tkn_update(newlst, lst, env) == 1)
				return (1);
		}
		else
			if (token_list_add(newlst, lst->type, lst->lexeme, \
					ft_strlen(lst->lexeme)) != 0)
				return (1);
		lst = lst->next;
	}
	return (0);
}

int	expand_redir_vars(t_token **newredir, t_token *lst, t_env *env)
{
	while (lst)
	{
		if (lst->type == WORD || (lst->type == D_QUOTE && expandable_dq(lst)) \
		|| lst->type == RD_IN_DQ || lst->type == RD_IN_WD \
		|| lst->type == RD_OUT_DQ || lst->type == RD_OUT_WD \
		|| lst->type == APPEND_DQ || lst->type == APPEND_WD)
		{
			if (tkn_update(newredir, lst, env) == 1)
				return (1);
		}
		else
			if (token_list_add(newredir, lst->type, lst->lexeme, \
					ft_strlen(lst->lexeme)) != 0)
				return (1);
		lst = lst->next;
	}
	return (0);
}

t_token	*list_expand(t_token *tokens, t_env *env)
{
	t_token	*newtknlst;
	int		*flagtab;
	int		**flagvec;

	newtknlst = NULL;
	flagvec = NULL;
	if (env)
	{
		if (expand_env_vars(&newtknlst, tokens, env) == 1)
			return (NULL);
		expand_tilde(newtknlst, env);
	}
	flagtab = create_wildflags(newtknlst);
	newtknlst = tkn_join(newtknlst);
	if (flagtab)
	{
		flagvec = create_wildvec(flagtab, newtknlst);
		free(flagtab);
		newtknlst = wildlst_expand(newtknlst, flagvec);
		clean_intvec(flagvec);
	}
	return (newtknlst);
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
		clean_intvec(flagvec);
	}
	return (newredir);
}

int	node_expand(t_cmd *cmd_node, t_env *env)
{
	if (cmd_node)
	{
		if (cmd_node->list)
		{
			cmd_node->list = list_expand(cmd_node->list, env);
			if (!cmd_node->list)
				return (1);
		}
		if (cmd_node->redir)
		{
			cmd_node->redir = redir_expand(cmd_node->redir, env);
			if (!cmd_node->redir)
				return (1);
		}
		if (cmd_node->sub_redir)
		{
			cmd_node->sub_redir = redir_expand(cmd_node->sub_redir, env);
			if (!cmd_node->sub_redir)
				return (1);
		}
		return (0);
	}
	return (1);
}
