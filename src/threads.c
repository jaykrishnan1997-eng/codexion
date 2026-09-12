/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 13:40:25 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/12 11:37:05 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int     create_threads(t_data *data)
{
	int	i;

	i = 0; 

    while (i < data->num_coders)
	{
        if (pthread_create(&data->coders[i].thread, NULL, coder_routine, &data->coders[i]) != 0) 
        {
			perror("Failed to create thread");
			return (-1);
		}
		i++;
    }
    if (pthread_create(&data->monitor_thread, NULL, monitor_routine, data) != 0)
	{
		perror("Failed to create monitor thread");
		return (-1);
	}
	return (0);
}

void    wait_for_threads(t_data *data)
{
    int i;

    i = 0;
    while (i < data->num_coders)
    {
        if (pthread_join(data->coders[i].thread, NULL) != 0) 
            perror("Failed to join thread");
		i++;
    }
    if (pthread_join(data->monitor_thread, NULL) != 0) 
        perror("Failed to join monitor thread");
}

