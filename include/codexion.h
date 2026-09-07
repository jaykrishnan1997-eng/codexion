/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 10:14:22 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/07 12:54:32 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>

typedef struct s_data t_data;

typedef struct s_dongle {
    int             dongle_id;
    pthread_mutex_t	mutex;
	pthread_cond_t  cond;
    int             in_use;
    long            available_at; // timestamp (ms) when cooldown ends
	t_heap			request_heap;
} t_dongle;

typedef struct s_heap {
    t_head_node *nodes;
    int         size;
    int         capacity;
} t_heap;

typedef	struct	s_coder {
    int 	    coder_id;
    int	        no_of_compiles;
    char	    *current_state;

	pthread_t	thread;
	
	
    t_dongle	*left_dongle;
	t_dongle    *right_dongle;
	t_data	    *data;
    long        last_compile_start;
} t_coder;

typedef	struct	s_data {
    int         num_coders;
    int         time_to_burnout;
    int         time_to_compile;
    int         time_to_debug;
    int         time_to_refractor;
    int	        required_compiles;
    int	        dongle_cooldown;

	char	    *scheduler;
	
    t_dongle    *dongles;
	t_coder		*coders;
	
    long        start_time;
	int			simulation_over;
    pthread_mutex_t log_mutex;
    pthread_mutex_t sim_mutex; // to protect simulation_over
} t_data;

typedef struct  s_request {
    t_coder 			*coder;
    struct  s_request 	*next;
} t_request;


// /* heap */
void	heap_insert();
void	heap_extract_min();

/* initialization */
int     init_data(t_data *data);
int     init_coders(t_data *data);
t_dongle *init_dongles(t_data *data);

/* threads */
int     create_threads(t_data *data);
void    wait_for_threads(t_data *data);

/* routine */
void    *coder_routine(void *arg);

/* actions */
void    take_dongles(t_coder *coder);
void    compile(t_coder *coder);
void    release_dongles(t_coder *coder);

/* monitoring */
void    *monitor_routine(void *arg);

/* mutex */
int     init_mutexes(t_data *data);
void    destroy_mutexes(t_data *data);

/* utilities */
long    get_time(void);
void    precise_sleep(int milliseconds);

/* cleanup */
void    cleanup(t_data *data);

#endif