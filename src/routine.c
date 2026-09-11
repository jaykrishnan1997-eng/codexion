/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 10:18:29 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/11 10:15:11 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <unistd.h>

static int	is_simulation_over(t_data *data) {
    int simulation_status;
	
	pthread_mutex_lock(&data->sim_mutex);
	simulation_status = data->simulation_over;
	pthread_mutex_unlock(&data->sim_mutex);
	return (simulation_status);
}

void    *coder_routine(void *arg) {
    
	t_coder *coder;

	coder = (t_coder *)arg;
	while(!is_simulation_over(coder->data)) {
		take_dongles(coder);
		coder->last_compile_start = get_time();
		log_state(coder, "is compiling");
		usleep(coder->data->time_to_compile * 1000);
		coder->no_of_compiles++;
		release_dongles(coder);
		
		log_state(coder, "is debugging");
		usleep(coder->data->time_to_debug * 1000);
		
		log_state(coder, "is refractoring");
		usleep(coder->data->time_to_refractor * 1000);
	}
    return (NULL);
}

