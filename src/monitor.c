/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 11:47:54 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/14 10:39:19 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_burnout(t_data *data)
{
	int		i;
	t_coder	*coder;
	long	elapsed;

	i = 0;
	while (i < data->num_coders)
	{
		coder = &data->coders[i];
		pthread_mutex_lock(&coder->state_mutex);
		elapsed = get_time() - coder->last_compile_start;
		pthread_mutex_unlock(&coder->state_mutex);
		if (elapsed >= data->time_to_burnout)
		{
			log_state(coder, "burned out");
			return (1);
		}
		i++;
	}
	return (0);
}

static int	check_all_done(t_data *data)
{
	int	j;
	int	all_done;
	int	compiles;

	j = 0;
	all_done = 1;
	while (j < data->num_coders)
	{
		pthread_mutex_lock(&data->coders[j].state_mutex);
		compiles = data->coders[j].no_of_compiles;
		pthread_mutex_unlock(&data->coders[j].state_mutex);
		if (compiles < data->required_compiles)
			all_done = 0;
		j++;
	}
	return (all_done);
}

void	*monitor_routine(void *arg)
{
	t_data	*data;

	data = (t_data *)arg;
	while (1)
	{
		if (check_burnout(data) || check_all_done(data)
			|| is_simulation_over(data))
		{
			pthread_mutex_lock(&data->sim_mutex);
			data->simulation_over = 1;
			pthread_mutex_unlock(&data->sim_mutex);
			return (NULL);
		}
		usleep(3000);
	}
}
