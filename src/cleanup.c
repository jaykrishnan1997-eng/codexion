/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 09:01:19 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/11 11:39:55 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdlib.h>

void    cleanup(t_data *data)
{
    int i;

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
	pthread_mutex_destroy(&data->log_mutex);
	pthread_mutex_destroy(&data->sim_mutex);
	pthread_mutex_destroy(&data->sequence_mutex);
}
