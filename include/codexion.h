/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 10:14:22 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/12 11:34:38 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <time.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <errno.h>
# include <sys/time.h>
# include <stdbool.h>

typedef struct  s_data t_data;
typedef struct  s_coder t_coder;

typedef struct s_heap_node {
	t_coder *coder;
	long	sequence;
	long	deadline;
} t_heap_node;

typedef struct s_heap {
    t_heap_node *nodes;
    int         size;
    int         capacity;
} t_heap;

typedef struct s_dongle {
    int             dongle_id;
    pthread_mutex_t	mutex;
	pthread_cond_t  cond;
    int             in_use;
    long            available_at; // timestamp (ms) when cooldown ends
	t_heap			request_heap;
} t_dongle;

typedef	struct	s_coder {
    int 	        coder_id;
    int	            no_of_compiles;
    char	        *current_state;
	pthread_t	    thread;
    pthread_mutex_t  state_mutex;
    t_dongle	    *left_dongle;
	t_dongle        *right_dongle;
	t_data	        *data;
    long            last_compile_start;
} t_coder;

typedef	struct	s_data {
    int             num_coders;
    int             time_to_burnout;
    int             time_to_compile;
    int             time_to_debug;
    int             time_to_refractor;
    int	            required_compiles;
    int	            dongle_cooldown;
	char	        *scheduler;
    t_dongle        *dongles;
	t_coder		    *coders;
    long            start_time;
	int			    simulation_over;
    pthread_mutex_t log_mutex; // to protect log data: prints to stdout/stderr
    pthread_mutex_t sim_mutex; // to protect simulation_over
    long            next_sequence;
    pthread_mutex_t sequence_mutex; // protect next sequence, since many coders increment it concurrently
    pthread_t       monitor_thread;
} t_data;

typedef struct  s_request {
    t_coder 			*coder;
    struct  s_request 	*next;
} t_request;


// /* heap */
void	    heap_insert(t_heap *heap, t_heap_node new_node, char *scheduler);
t_heap_node	heap_extract_min(t_heap *heap, char *scheduler);

/* initialization */
int     parser(int argc, char **argv, t_data *data);
t_dongle *init_dongles(t_data *data);
int     init_coders(t_data *data, t_dongle *dongles);

/* threads */
int     create_threads(t_data *data);
void    wait_for_threads(t_data *data);

/* routine */
void    *coder_routine(void *arg);
int	is_simulation_over(t_data *data);

/* actions */
int    take_dongles(t_coder *coder);
void    release_dongles(t_coder *coder);

/* monitoring */
void    *monitor_routine(void *arg);

/* mutex */
int     init_mutexes(t_data *data);
void    destroy_mutexes(t_data *data);

/* utilities */
long    get_time(void);
void    precise_sleep(int milliseconds);
void	log_state(t_coder *coder, char *message);

/* cleanup */
void    cleanup(t_data *data);

#endif