/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 20:05:09 by daflynn           #+#    #+#             */
/*   Updated: 2025/09/17 14:30:00 by daflynn          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*append_exit_status(char *res, int status, int *i)
{
	char	*temp;

	temp = ft_itoa(status);
	res = ft_strjoin_free(res, temp);
	free(temp);
	*i += 2;
	return (res);
}

void	ft_exit_numeric_error(char *arg)
{
	ft_putstr_fd("exit: ", 2);
	if (arg)
	{
		ft_putstr_fd(arg, 2);
		ft_putstr_fd(": ", 2);
	}
	ft_putendl_fd("numeric argument required", 2);
}

int	ft_is_valid_exit_arg(char *str)
{
	int	i;
	int	digits;

	i = 0;
	digits = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		digits++;
		i++;
	}
	return (digits > 0);
}

void	ft_exit(t_dat *data, size_t k)
{
	int	status;

	if (data->xln[k + 1] && data->xln[k + 2])
	{
		ft_putendl_fd("exit: too many arguments", 2);
		g_last_exit_status = 1;
		return ;
	}
	rl_clear_history();
	if (data->xln[k + 1] == NULL)
		ft_cleanup_exit(data, g_last_exit_status);
	if (!ft_is_valid_exit_arg(data->xln[k + 1]))
	{
		ft_exit_numeric_error(data->xln[k + 1]);
		ft_cleanup_exit(data, 2);
	}
	status = ft_atoi(data->xln[k + 1]);
	ft_cleanup_exit(data, status % 256);
}

char	*ft_expand_exit_status(t_dat *d, char *token)
{
	char *res;
	int i;

	(void)d;
	i = 0;
	res = malloc(1);
	if (!res)
		return (NULL);
	res[0] = '\0';
	while (token[i])
	{
		if (token[i] == '$' && token[i + 1] == '?')
			res = append_exit_status(res, g_last_exit_status, &i);
		else
			res = append_char(res, token, &i);
	}
	return (res);
}