/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 09:01:19 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/11 09:20:48 by jkrishna         ###   ########.fr       */
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
        i++;    
    }
    free(data->dongles);
    free(data->coders);
	pthread_mutex_destroy(&data->log_mutex);
	pthread_mutex_destroy(&data->sim_mutex);
	pthread_mutex_destroy(&data->sequence_mutex);
}
