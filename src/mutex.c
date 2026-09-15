/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 11:01:33 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/15 11:30:14 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_mutexes(t_data *data)
{
	if (pthread_mutex_init(&data->log_mutex, NULL) != 0)
		return (-1);
	if (pthread_mutex_init(&data->sim_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&data->log_mutex);
		return (-1);
	}
	if (pthread_mutex_init(&data->sequence_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&data->log_mutex);
		pthread_mutex_destroy(&data->sim_mutex);
		return (-1);
	}
	data->next_sequence = 0;
	return (0);
}

void	destroy_mutexes(t_data *data)
{
	pthread_mutex_destroy(&data->log_mutex);
	pthread_mutex_destroy(&data->sim_mutex);
	pthread_mutex_destroy(&data->sequence_mutex);
}
