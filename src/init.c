/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 11:19:56 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/04 15:13:01 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>
#include <stdlib.h>
#include <string.h>


t_data parser(int argc, char **argv)
{
    // initalize data storage
    typedef t_data *data;

    int i;

    // check data arguments if invalid (not positive integers)
    // 8th argument is a char*
    for (i = 1; i < argc - 2; i++) {
        if ((atoi(argv[i]) == 0) && argv[i] != "0")  {
            printf("Invalid input at %dth argument!", i + 1);
            return -1;
        }
        if (atoi(argv[i]) <= 0) {
            printf("%dth argument is not acceptable. Must be positive integer", i + 1);
            return -1;
        }
    }

    // consuming parameters for data
    data->num_coders = atoi(argv[1]); // nc
    data->time_to_burnout = atoi(argv[2]); // tb
    data->time_to_compile = atoi(argv[3]); // tc
    data->time_to_debug = atoi(argv[4]); // td
    data->time_to_refractor = atoi(argv[5]); // tr
    data->required_compiles = atoi(argv[6]); // ncr
    data->dongle_cooldown = atoi(argv[7]); // dc

    data->scheduler = argv[8]; // sch
    
    if strcmp(data->scheduler)


}
    // printf("./codexion.c nc tb tc td tr ncr dc sch")
