/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 10:14:22 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/04 11:05:57 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>

typedef struct s_data t_data;

typedef	struct	s_coder {
    long	num_coders
    long	burnout_time
    long	compile_time
    long	debug_time
    long	refractor_time
    long	no_of_compiles
    char	current_state

	pthread_t	thread;
	
	pthread_mutex_t	*left_dongle;
	pthread_mutex_t *right_dongle;
	
	t_data	*data;
} t_coder;

typedef	struct	s_data {
    int		coder_id
    long	time_to_burnout
    long	time_to_compile
    long	time_to_debug
    long	time_to_refractor
    long	required_compiles
    long	dongle_cooldown

	char	scheduler
	
	t_coder	*coders;
	
	int	simulation_over;
} t_data;

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
// long    get_time(void);
// void    precise_sleep(long milliseconds);

// /* cleanup */
// void    cleanup(t_data *data);

// #endif