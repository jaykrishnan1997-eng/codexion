/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 11:01:33 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/11 10:14:38 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_mutexes(t_data *data) {
    if (pthread_mutex_init(&data->log_mutex, NULL) != 0)
        return (-1);
    if (pthread_mutex_init(&data->sim_mutex, NULL) != 0)
        return (-1);
    if (pthread_mutex_init(&data->sequence_mutex, NULL) != 0)
        return (-1);
    data->next_sequence = 0;
    return (0);
}

void	destroy_mutexes(t_data *data) {
	pthread_mutex_destroy(&data->log_mutex);
	pthread_mutex_destroy(&data->sim_mutex);
	pthread_mutex_destroy(&data->sequence_mutex);
}

// log_mutex: protects the "only one thread to print to stdout/stderr at a time"
// sim_mutex: protects the "simulation_over" flag
// sequence_mutex: protects "next_sequence", since many coders increament it concurrently when making requests.
// each dongle's own mutex protects dongle's in_use / available_at/heap state specifically