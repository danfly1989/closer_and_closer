/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 12:00:00 by daflynn           #+#    #+#             */
/*   Updated: 2025/09/17 12:00:00 by daflynn          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * Remove empty strings from d->xln
 */
void	ft_rmv_empty_str_from_xln(t_dat *d)
{
	size_t	i;
	size_t	j;

	if (!d || !d->xln)
		return ;
	i = 0;
	while (d->xln[i])
	{
		if (d->xln[i][0] == '\0')
		{
			free(d->xln[i]);
			j = 1;
			while (d->xln[i + j])
			{
				d->xln[i] = d->xln[i + j];
				j++;
			}
			d->xln[i + j - 1] = NULL; // terminate properly
		}
		else
			i++;
	}
}
void	ft_strip_quotes_within_str(char *s)
{
	size_t	i;
	size_t	j;
	char	in_quote;

	if (!s)
		return ;
	i = 0;
	j = 0;
	in_quote = 0;
	while (s[i])
	{
		if (!in_quote && (s[i] == '\'' || s[i] == '"'))
		{
			in_quote = s[i];
		}
		else if (in_quote && s[i] == in_quote)
		{
			in_quote = 0;
		}
		else
		{
			s[j++] = s[i];
		}
		i++;
	}
	s[j] = '\0';
}

void	ft_strip_surrounding_quotes(char *s)
{
	size_t	len;
	char	first_char;
	char	last_char;

	if (!s)
		return ;
	len = ft_strlen(s);
	if (len < 2)
		return ;
	first_char = s[0];
	last_char = s[len - 1];
	// Only remove if both ends have matching quotes
	if ((first_char == '"' && last_char == '"') || (first_char == '\''
			&& last_char == '\''))
	{
		ft_memmove(s, s + 1, len - 2);
		s[len - 2] = '\0';
	}
}

void	ft_strip_quotes_after_equal(char *s)
{
	char	*eq;
	size_t	len;
	char	first_quote;
	char	last_char;

	if (!s || !(eq = ft_strchr(s, '=')) || !eq[1])
		return ;
	len = ft_strlen(eq + 1);
	first_quote = eq[1];
	last_char = eq[len];
	if ((first_quote == '"' && last_char == '"') || (first_quote == '\''
			&& last_char == '\''))
	{
		ft_memmove(eq + 1, eq + 2, len - 1);
		eq[len] = '\0';
	}
}

void	ft_strip_quotes_from_xln(t_dat *d)
{
	size_t	i;

	if (!d || !d->xln)
		return ;
	// First remove empty strings
	ft_rmv_empty_str_from_xln(d);
	i = 0;
	while (d->xln[i])
	{
		// Handle assignment quotes first
		ft_strip_quotes_after_equal(d->xln[i]);
		// Then handle internal quotes
		ft_strip_quotes_within_str(d->xln[i]);
		// Finally handle surrounding quotes
		ft_strip_surrounding_quotes(d->xln[i]);
		i++;
	}
}

/*
 * Detect type of quote for a token
 */
void	ft_detect_quote_type(char *token, int *quote_type)
{
	char	*eq;
	char	quote;

	if (!token || !quote_type)
		return ;
	if (token[0] == '\'')
		*quote_type = 1;
	else if (token[0] == '"')
		*quote_type = 2;
	else
	{
		eq = ft_strchr(token, '=');
		if (eq && (*(eq + 1) == '\'' || *(eq + 1) == '"'))
		{
			quote = *(eq + 1);
			*quote_type = (quote == '\'') ? 1 : 2;
		}
	}
}

int	ft_skip_quote(char *str, int i)
{
	char	q;

	if (!str || (str[i] != '\'' && str[i] != '"'))
		return (i);
	q = str[i];
	i++; /* skip opening quote */
	while (str[i] && str[i] != q)
	{
		if (q == '"' && str[i] == '\\' && str[i + 1])
			i += 2; /* allow \" or escaped char inside double quotes */
		else
			i++;
	}
	if (str[i] == q)
		i++; /* skip closing quote */
	return (i);
}
