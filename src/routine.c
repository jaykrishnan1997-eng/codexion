/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 10:18:29 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/14 12:13:56 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_simulation_over(t_data *data)
{
	int	simulation_status;

	pthread_mutex_lock(&data->sim_mutex);
	simulation_status = data->simulation_over;
	pthread_mutex_unlock(&data->sim_mutex);
	return (simulation_status);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (!is_simulation_over(coder->data))
	{
		if (take_dongles(coder) == -1)
			break ;
		pthread_mutex_lock(&coder->state_mutex);
		coder->last_compile_start = get_time();
		pthread_mutex_unlock(&coder->state_mutex);
		log_state(coder, "is compiling");
		if (interruptible_sleep(coder, coder->data->time_to_compile) == -1)
		{
			release_dongles(coder);
			break ;
		}
		pthread_mutex_lock(&coder->state_mutex);
		coder->no_of_compiles++;
		pthread_mutex_unlock(&coder->state_mutex);
		release_dongles(coder);
		log_state(coder, "is debugging");
		if (interruptible_sleep(coder, coder->data->time_to_debug) == -1)
			break ;
		log_state(coder, "is refactoring");
		if (interruptible_sleep(coder, coder->data->time_to_refractor) == -1)
			break ;
	}
	return (NULL);
}
