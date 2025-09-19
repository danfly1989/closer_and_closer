/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daflynn <daflynn@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 19:56:15 by daflynn           #+#    #+#             */
/*   Updated: 2025/09/13 19:56:21 by daflynn          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_appropriate_child_signal(char *str)
{
	if (ft_is_builtin(str) && ft_is_pipe_builtin(str))
		ft_set_main_signals();
	else
		ft_set_child_signals();
}

void	ft_child_process(t_dat *d, char ***cmd, int **fd, size_t i)
{
	t_rdr	r;
	int		len;

	if (!cmd || !cmd[i] || !cmd[i][0])
		exit(1);
	len = 0;
	while (cmd[i][len])
		len++;
	ft_memset(&r, 0, sizeof(r));
	if (!ft_validate_segment(cmd[i], 0, len))
		exit(1);
	ft_setup_io(fd, i, d->tot);
	ft_close_pipes(fd, d->tot);
	ft_appropriate_child_signal(cmd[i][0]);
	ft_parse_redirection(cmd[i], &r);
	if (!ft_apply_redirections(&r, cmd[i]))
	{
		ft_free_redirection(&r);
		exit(1);
	}
	ft_exec_command(d, cmd[i]);
}

// Modified ft_fork_children to return array of PIDs
pid_t	*ft_fork_children(t_dat *d, char ***cmd, int **fd)
{
	pid_t	*pids;
	size_t	i;

	pids = malloc(sizeof(pid_t) * d->tot);
	if (!pids)
		return (NULL);
	i = 0;
	while (i < d->tot)
	{
		pids[i] = -1;
		if (!cmd[i] || !cmd[i][0])
		{
			i++;
			continue ;
		}
		pids[i] = fork();
		if (pids[i] == 0)
		{
			ft_set_child_signals();
			ft_child_process(d, cmd, fd, i);
		}
		else if (pids[i] < 0)
			perror("fork");
		i++;
	}
	return (pids);
}

// Modified ft_wait_children to handle last command specifically
void	ft_wait_children(pid_t *pids, int tot, int last_index)
{
	int		status;
	int		i;
	pid_t	pid;

	i = 0;
	while (i < tot)
	{
		if (pids[i] != -1)
		{
			pid = waitpid(pids[i], &status, 0);
			if (pid == pids[last_index])
			{
				if (WIFEXITED(status))
					g_last_exit_status = WEXITSTATUS(status);
				else if (WIFSIGNALED(status))
					g_last_exit_status = 128 + WTERMSIG(status);
			}
		}
		i++;
	}
}

void	ft_nested_child(t_dat *d, char **cmd, char *cmd_path, int s_stdin)
{
	close(s_stdin);
	cmd_path = ft_get_cmd_path(d, cmd[0], 0);
	if (!cmd_path)
	{
		ft_cmd_not_found(cmd[0]);
		exit(1);
	}
	execve(cmd_path, cmd, d->evs);
	exit(1);
}
