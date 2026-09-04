/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 10:14:22 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/04 14:38:19 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <heapq>

typedef struct s_data t_data;

typedef	struct	s_coder {
    int 	coder_id
    int 	burnout_time
    int	    compile_time
    int	    debug_time
    int	    refractor_time
    int	    no_of_compiles
    char	*current_state

	pthread_t	thread;
	
	pthread_mutex_t	*left_dongle;
	pthread_mutex_t *right_dongle;
	
	t_data	*data;
} t_coder;

typedef	struct	s_data {
    int     num_coders
    int     time_to_burnout
    int     time_to_compile
    int     time_to_debug
    int     time_to_refractor
    int	    required_compiles
    int	    dongle_cooldown

	char	*scheduler
	
	t_coder	*coders;
	
	int	simulation_over;
} t_data;

typedef struct  s_request {
    t_coder *coder;
    struct  s_request *next;
} t_request;

#endif



// /* initialization */
// int     init_data(t_data *data);

// /* threads */
// int     create_threads(t_data *data);
// void    wait_for_threads(t_data *data);

// /* routine */
// void    *philosopher_routine(void *arg);

// /* actions */
// void    take_forks(t_philo *philo);
// void    eat(t_philo *philo);
// void    release_forks(t_philo *philo);

// /* monitoring */
// void    *monitor_routine(void *arg);

// /* mutex */
// int     init_mutexes(t_data *data);
// void    destroy_mutexes(t_data *data);

// /* utilities */
// int    get_time(void);
// void    precise_sleep(int milliseconds);

// /* cleanup */
// void    cleanup(t_data *data);

// #endif