/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 11:19:56 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/07 09:55:56 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int  is_valid_number(const char *str){
    int i;

    i = 0;
    if (str[i] == '\0')
        return (0);
    
    while (str[i]) {
        if (str[i] < '0' || str[i] > '9')
            return (0);
        i++;
    }
    return (1);
}

int parser(int argc, char **argv, t_data *data)
{

    int i;

    i = 1;
    // check data arguments if invalid (not positive integers)
    // 8th argument is a char*
    while (i <= 7) {
        
        if (!is_valid_number(argv[i])) {

            printf("Argument %d must be a positive integer\n", i);
            return (-1);
        }

        if (atoi(argv[i]) <= 0) {

            printf("Argument %d must be greater than zero\n", i);
            return (-1);
        }
        i++;
    }
    if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0) {
        
        printf("Scheduler must be exactly 'fifo' or 'edf'\n");
        return (-1);
    }

    // consuming parameters for data
    // printf("./codexion.c nc tb tc td tr ncr dc sch")
    data->num_coders = atoi(argv[1]); // nc
    data->time_to_burnout = atoi(argv[2]); // tb
    data->time_to_compile = atoi(argv[3]); // tc
    data->time_to_debug = atoi(argv[4]); // td
    data->time_to_refractor = atoi(argv[5]); // tr
    data->required_compiles = atoi(argv[6]); // ncr
    data->dongle_cooldown = atoi(argv[7]); // dc
    data->scheduler = argv[8]; // sch
    
    return (0);
}

t_dongle	*init_dongles(t_data *data) {
    int			d;
    // list of pointers with address to dongles
    t_dongle 	*dongles;
	
	dongles = malloc(sizeof(t_dongle) * data->num_coders);
	if (!dongles)
		return (NULL);
    d = 0;

    while (d < data->num_coders)
    {
		pthread_mutex_init(&dongles[d].mutex, NULL);
    	pthread_cond_init(&dongles[d].cond, NULL);
		dongles[d].in_use = 0;
		dongles[d].available_at = 0;
		d++;
    }
    return (dongles);
}

int	init_coders(t_data *data, t_dongle *dongles) {
    int c;
    // list of pointers with address to dongles
	
	data->coders = malloc(sizeof(t_coder) * data->num_coders);
	if (!data->coders)
		return (-1);
    c = 0;
    while (c < data->num_coders)
    {
        data->coders[c].coder_id = c;
		data->coders[c].no_of_compiles = 0;
		data->coders[c].current_state = "idle";
		data->coders[c].left_dongle = &dongles[c];
		data->coders[c].right_dongle = &dongles[(c + 1) % data->num_coders];
		data->coders[c].data = data;
		data->coders[c].last_compile_start = 0;
		c++;
    }
    return (0);
}

    // Call both of these from main() after parser() succeed