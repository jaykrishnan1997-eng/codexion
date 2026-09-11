/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 11:47:54 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/11 11:49:10 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <unistd.h>

void    *monitor_routine(void *arg) {
    t_data  *data;
    t_coder *coder;
    int i;
    int j;
    int all_done;
    long elapsed;
    int compiles;
    
    data = (t_data *)arg;
    while (1)
    {
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
                pthread_mutex_lock(&data->sim_mutex);
                data->simulation_over = 1;
                pthread_mutex_unlock(&data->sim_mutex);
                return (NULL);
            }
            i++;
        }
        j = 0;
        all_done = 1;
        while (j < data->num_coders)
        {
            coder = &data->coders[j];
			pthread_mutex_lock(&coder->state_mutex);
			compiles = coder->no_of_compiles;
			pthread_mutex_unlock(&coder->state_mutex); 
            if (compiles < data->required_compiles)
                all_done = 0;
            j++;
        }
        if (all_done)
        {
            pthread_mutex_lock(&data->sim_mutex);
            data->simulation_over = 1;
            pthread_mutex_unlock(&data->sim_mutex);
            return (NULL);
        }
        if (is_simulation_over(data))
            return (NULL);
        usleep(3000);
    }
}
