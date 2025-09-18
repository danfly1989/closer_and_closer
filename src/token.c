/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* token.c                                            :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/09/13 19:28:57 by daflynn           #+#    #+#             */
/* Updated: 2025/09/13 19:29:04 by daflynn          ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "minishell.h"

// Skips over a single token, including quoted sections.
// It relies on ft_skip_quote to correctly navigate nested quotes.
int	ft_skip_token(char *str, int i)
{
	while (str[i] && str[i] != ' ')
	{
		if (str[i] == '\'' || str[i] == '"')
			i = ft_skip_quote(str, i);
		else
			i++;
	}
	return (i);
}

// Splits the input string into an array of tokens,
// It also populates an array with the quote type of each token.
char	**ft_tokenize_line(t_dat *d, char *str, int **quote_types_out)
{
	char	**tokens;

	ft_reset_iterators(d);
	d->k = ft_count_tokens(str);
	tokens = malloc(sizeof(char *) * (d->k + 1));
	d->qtypes = malloc(sizeof(int) * (d->k + 1));
	if (!tokens || !d->qtypes)
	{
		// On allocation failure, free any allocated memory and return NULL.
		return (ft_free_token_quote(tokens, d->qtypes));
	}
	while (str[d->i])
	{
		while (str[d->i] == ' ')
			d->i++;
		if (!str[d->i])
			break ;
		tokens[d->j] = ft_extract_token(str, d, &d->qtypes[d->j]);
		d->j++;
	}
	tokens[d->j] = NULL;
	d->qtypes[d->j] = -1;
	*quote_types_out = d->qtypes;
	return (tokens);
}

// Finds the value of a variable from a linked list of variables.
// A key change was made here to ensure an exact name match.
char	*ft_get_var_value(t_va *list, const char *name)
{
	size_t	n;

	n = ft_strlen(name);
	while (list)
	{
		// Check for exact name match to avoid issues with partial matches
		// For example, finding "PATH" when "PATH_LONG" exists.
		if (ft_strlen(list->name) == n && ft_strncmp(list->name, name, n) == 0)
			return (list->value);
		list = list->next;
	}
	return (NULL);
}

// Joins a string with a single character, allocating a new string.
// Handles the case where the input string `s` is NULL.
char	*ft_strjoin_char(const char *s, char c)
{
	char *new;
	size_t len;

	if (!s)
	{
		new = malloc(2);
		if (!new)
			return (NULL);
		new[0] = c;
		new[1] = '\0';
		return (new);
	}
	len = ft_strlen(s);
	new = malloc(len + 2);
	if (!new)
		return (NULL);
	ft_strlcpy(new, s, len + 1);
	new[len] = c;
	new[len + 1] = '\0';
	return (new);
}