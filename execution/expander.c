/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaadiou <hlaadiou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/15 08:13:22 by hlaadiou          #+#    #+#             */
/*   Updated: 2023/08/06 09:06:23 by hlaadiou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_token	*tkn_join(t_token *lst)
{
	t_token	*joined;
	char	*newlex;
	char	*tmp;

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
			token_list_add(&joined, WORD, newlex, ft_strlen(newlex));
		if (lst)
		{
			token_list_add(&joined, lst->type, lst->lexeme, 1);
			lst = lst->next;
		}
		free(newlex);
	}
	return (joined);
}

// Update the token if it is of type WORD or D_QUOTE
// into a new expanded token added to the 'newtknlst'.
void	tkn_update(t_token **newlst, t_token *lst, t_env *env)
{
	char	*var;
	t_token	*sub;

	sub = tkn_split(lst);
	if (!sub)
		return ;
	while (sub && sub->lexeme)
	{
		var = sub->lexeme;
		if (*var == '$' && *(var + 1))
			var = get_env_value(env, (var + 1));
		else if (*var == '$' && lst->next \
		&& (lst->next->type == D_QUOTE || lst->next->type == S_QUOTE))
			var = NULL;
		if (var)
			token_list_add(newlst, sub->type, var, ft_strlen(var));
		sub = sub->next;
	}
	return ;
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
				if (tknlst->type == D_QUOTE || tknlst->type == S_QUOTE)
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
	//for (int i = 0; i < wildchars; i++)
	//	printf("%d\n", flags[i]);
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

void	wildlex_match(t_token **lst, t_entry *matches, char *dir)
{
	char	*wildlex;

	while (matches)
	{
		if (dir)
			wildlex = ft_strjoin(dir, matches->name);
		else
			wildlex = matches->name;
		token_list_add(lst, WORD, wildlex, ft_strlen(wildlex));
		if (dir)
			free(wildlex);
		matches = matches->next;
	}
	return ;
}

t_token	*wild_expand(t_token *tknlst, int *flags)
{
	t_token	*wildtknlst;
	t_entry	*matches;
	char	*pattern;
	char	*dir;

	wildtknlst = NULL;
	while (tknlst)
	{
		dir = NULL;
		pattern = NULL;
		if (tknlst->type == WORD)
		{
			extract_dir_pattern(&dir, &pattern, tknlst);
			matches = dir_pattern_check(dir, pattern, flags);
			if (matches)
				wildlex_match(&wildtknlst, matches, dir);
			else
				token_list_add(&wildtknlst, WORD, tknlst->lexeme, \
						ft_strlen(tknlst->lexeme));
			free(pattern);
			free(dir);
			clean_list(&matches);
		}
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

void	expand_tilde(t_token *lst, t_env *env)
{
	char	*home;
	char	*newlex;

	if (!lst)
		return ;
	home = get_home(env);
	if (!home)
		return ;
	while (lst)
	{
		newlex = NULL;
		if (lst->type == WORD && *lst->lexeme == '~')
		{
			if (!*(lst->lexeme + 1) && \
				(!lst->next || (lst->next && lst->next->type == SPC)))
				newlex = ft_strdup(home);
			else if (*(lst->lexeme + 1) && *(lst->lexeme + 1) == '/')
				newlex = ft_strjoin(home, lst->lexeme + 1);
			if (newlex)
			{
				lst->lexeme = newlex;
				garbage_list_add(&g_gc, newlex);
			}
		}
		lst = lst->next;
	}
	free(home);
}

t_token	*list_expand(t_token *tokens, t_env *env)
{
	t_token	*token;
	t_token	*newtknlst;
	int		*flags_tab;

	token = tokens;
	newtknlst = NULL;
	while (token)
	{
		if (token->type == WORD || (token->type == D_QUOTE && *token->lexeme))
			tkn_update(&newtknlst, token, env);
		else
			token_list_add(&newtknlst, token->type, token->lexeme, \
					ft_strlen(token->lexeme));
		token = token->next;
	}
	expand_tilde(newtknlst, env);
	flags_tab = create_wildflags(newtknlst);
	newtknlst = tkn_join(newtknlst);
	if (flags_tab)
		newtknlst = wild_expand(newtknlst, flags_tab);
	free(flags_tab);
	return (newtknlst);
}

void	node_expand(t_cmd *cmd_node, t_env *env)
{
	if (cmd_node)
	{
		cmd_node->list = list_expand(cmd_node->list, env);
		//cmd_node->in = rdir_expand(cmd_node->in);
		//cmd_node->out = rdir_expand(cmd_node->out);
	}
	return ;
}
