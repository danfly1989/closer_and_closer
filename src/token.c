/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 19:28:57 by daflynn           #+#    #+#             */
/*   Updated: 2025/09/13 19:29:04 by daflynn          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ft_extract_special_token(char *str, t_dat *d)
{
	char	*token;
	int		len;

	// Handle >> append operator
	if (str[d->i] == '>' && str[d->i + 1] == '>')
	{
		len = 2;
		token = malloc(3);
		ft_strlcpy(token, &str[d->i], 3);
	}
	// Handle other special characters
	else
	{
		len = 1;
		token = malloc(2);
		token[0] = str[d->i];
		token[1] = '\0';
	}
	d->i += len;
	return (token);
}

int	is_special_char(char c)
{
	return (c == '<' || c == '>' || c == '|' || c == ';' || c == '&');
}

int	ft_skip_token(char *str, int i)
{
	int		in_quotes;
	char	quote_char;

	in_quotes = 0;
	quote_char = 0;
	while (str[i] && (in_quotes || (str[i] != ' ' && !is_special_char(str[i]))))
	{
		if (!in_quotes && (str[i] == '\'' || str[i] == '"'))
		{
			in_quotes = 1;
			quote_char = str[i];
		}
		else if (in_quotes && str[i] == quote_char)
		{
			in_quotes = 0;
		}
		i++;
	}
	return (i);
}
char	**ft_tokenize_line(t_dat *d, char *str, int **quote_types_out)
{
	char	**tokens;

	ft_reset_iterators(d);
	d->k = ft_count_tokens(str);
	tokens = malloc(sizeof(char *) * (d->k + 1));
	d->qtypes = malloc(sizeof(int) * (d->k + 1));
	if (!tokens || !d->qtypes)
		return (ft_free_token_quote(tokens, d->qtypes));
	while (str[d->i])
	{
		while (str[d->i] == ' ')
			d->i++;
		if (!str[d->i])
			break ;
		// Handle special characters as separate tokens
		if (is_special_char(str[d->i]) && (d->i == 0 || str[d->i - 1] != '\\'))
		{
			tokens[d->j] = ft_extract_special_token(str, d);
			d->qtypes[d->j] = 0; // Special tokens aren't quoted
			d->j++;
			continue ;
		}
		tokens[d->j] = ft_extract_token(str, d, &d->qtypes[d->j]);
		d->j++;
	}
	tokens[d->j] = NULL;
	d->qtypes[d->j] = -1;
	*quote_types_out = d->qtypes;
	return (tokens);
}

char	*ft_get_var_value(t_va *list, const char *name)
{
	size_t	n;

	n = ft_strlen(name);
	while (list)
	{
		if (list->name && ft_strlen(list->name) == n && ft_strncmp(list->name,
				name, n) == 0)
			return (list->value);
		list = list->next;
	}
	return (NULL);
}

char	*ft_strjoin_char(const char *s, char c)
{
	char *new;
	size_t len;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	new = malloc(len + 2);
	if (!new)
		return (NULL);
	ft_strlcpy(new, s, len + 1);
	new[len] = c;
	new[len + 1] = '\0';
	return (new);
}