/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 14:47:54 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/15 11:07:26 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_time(void);

int	interruptible_sleep(t_coder *coder, int time)
{
	long	end;
	long	remaining;
	int		chunk;
	int		status;

	status = 0;
	end = get_time() + time;
	while (get_time() < end)
	{
		if (is_simulation_over(coder->data))
			return (-1);
		remaining = end - get_time();
		chunk = 2;
		if (remaining < chunk)
			chunk = (int)remaining;
		if (chunk > 0)
			usleep(chunk * 1000);
	}
	if (is_simulation_over(coder->data))
		status = -1;
	return (status);
}

bool	checkin(char c, char *str)
{
	int	i;
	int	length;

	i = 0;
	length = strlen(str);
	while (i < length)
	{
		if (c == str[i])
			return (true);
		i++;
	}
	return (false);
}

long	get_time(void)
{
	long			time_in_ms;
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	time_in_ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	return (time_in_ms);
}

void	log_state(t_coder *coder, char *message)
{
	pthread_mutex_lock(&coder->data->log_mutex);
	printf(
		"%ld %d %s\n", get_time() - coder->data->start_time,
		coder->coder_id + 1, message);
	pthread_mutex_unlock(&coder->data->log_mutex);
}

void	cleanup(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_coders)
	{
		pthread_mutex_destroy(&data->dongles[i].mutex);
		pthread_cond_destroy(&data->dongles[i].cond);
		free(data->dongles[i].request_heap.nodes);
		pthread_mutex_destroy(&data->coders[i].state_mutex);
		i++;
	}
	free(data->dongles);
	free(data->coders);
	destroy_mutexes(data);
}
