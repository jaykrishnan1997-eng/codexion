/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 11:19:56 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/05 12:28:19 by jkrishna         ###   ########.fr       */
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

    
    // Write an init_dongles() function (in a new file, e.g. init_dongles.c) that:
    // Allocates the dongle array (malloc, size = num_coders).
    // Loops through and calls pthread_mutex_init and pthread_cond_init on each dongle, 
    // sets in_use = 0, available_at = 0.

    // Returns success/failure (handle malloc failure).



    // Write an init_coders() function that allocates the coder array and wires up each 
    // coder's left_dongle/right_dongle pointers into the shared dongle array (remember: 
    // coder i's left dongle and coder i-1's right dongle should be the same dongle — that's
    //  the whole point of the shared pool).
    
    // Call both of these from main() after parser() succeed