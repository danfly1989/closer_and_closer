/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 20:02:33 by daflynn           #+#    #+#             */
/*   Updated: 2025/09/13 20:02:39 by daflynn          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_apply_redirections(t_rdr *r, char **tokens)
{
	int	last_in_fd;
	int	last_out_fd;
	int	i;
	int	fd;

	last_in_fd = -1;
	last_out_fd = -1;
	i = 0;
	// Handle heredoc first if present
	if (r->heredoc_delim)
	{
		fd = handle_heredoc(r->heredoc_delim);
		if (fd == -1)
			return (0);
		if (dup2(fd, STDIN_FILENO) == -1)
		{
			perror("dup2");
			close(fd);
			return (0);
		}
		close(fd);
	}
	// Process all redirection tokens
	while (tokens[i])
	{
		if (ft_strcmp(tokens[i], "<") == 0 && tokens[i + 1])
		{
			if (last_in_fd != -1)
				close(last_in_fd);
			last_in_fd = open(tokens[i + 1], O_RDONLY);
			if (last_in_fd < 0)
			{
				perror(tokens[i + 1]);
				return (0);
			}
			i += 2;
		}
		else if (ft_strcmp(tokens[i], ">") == 0 && tokens[i + 1])
		{
			if (last_out_fd != -1)
				close(last_out_fd);
			last_out_fd = open(tokens[i + 1], O_CREAT | O_WRONLY | O_TRUNC,
					0644);
			if (last_out_fd < 0)
			{
				perror(tokens[i + 1]);
				return (0);
			}
			i += 2;
		}
		else if (ft_strcmp(tokens[i], ">>") == 0 && tokens[i + 1])
		{
			if (last_out_fd != -1)
				close(last_out_fd);
			last_out_fd = open(tokens[i + 1], O_CREAT | O_WRONLY | O_APPEND,
					0644);
			if (last_out_fd < 0)
			{
				perror(tokens[i + 1]);
				return (0);
			}
			i += 2;
		}
		else
		{
			i++;
		}
	}
	// Apply the final redirections
	if (last_in_fd != -1)
	{
		if (dup2(last_in_fd, STDIN_FILENO) < 0)
		{
			perror("dup2 stdin");
			close(last_in_fd);
			return (0);
		}
		close(last_in_fd);
	}
	if (last_out_fd != -1)
	{
		if (dup2(last_out_fd, STDOUT_FILENO) < 0)
		{
			perror("dup2 stdout");
			close(last_out_fd);
			return (0);
		}
		close(last_out_fd);
	}
	return (ft_remove_redirections(&tokens, 0, 0));
}
void	ft_parent_sigint_handler(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	g_last_exit_status = 130;
}

int	ft_redir_in(char *file)
{
	int	fd;

	fd = open(file, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "minishell: %s: No such file or directory\n", file);
		return (0);
	}
	if (dup2(fd, STDIN_FILENO) < 0)
	{
		perror("dup2 in");
		close(fd);
		return (0);
	}
	close(fd);
	return (1);
}

int	ft_redir_out(char *file)
{
	int	fd;

	fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd < 0)
	{
		if (errno == EACCES)
		{
			fprintf(stderr, "minishell: %s: Permission denied\n", file);
		}
		else
		{
			perror(file);
		}
		return (0);
	}
	if (dup2(fd, STDOUT_FILENO) < 0)
	{
		perror("dup2 out");
		close(fd);
		return (0);
	}
	close(fd);
	return (1);
}

int	ft_redir_append(char *file)
{
	int	fd;

	fd = open(file, O_CREAT | O_WRONLY | O_APPEND, 0644);
	if (fd < 0)
		return (perror(file), 0);
	if (dup2(fd, STDOUT_FILENO) < 0)
		return (perror("dup2 append"), 0);
	close(fd);
	return (1);
}
