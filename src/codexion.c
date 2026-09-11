/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 15:51:15 by jay-k             #+#    #+#             */
/*   Updated: 2026/09/11 10:26:28 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

int main(int argc, char **argv){

	int			i;
    t_data		data;
	t_dongle	*dongles;

    if (argc != 9)
    {
		printf("Correct usage is:\n");
		printf("./codexion.c nc tb tc td tr ncr dc sch\n");
		return (-1);
	}
    if (parser(argc, argv, &data) == -1)
    {
		return (-1);	
	}
	dongles = init_dongles(&data);
	if (!dongles)
		return (-1);
	data.dongles = dongles;
	if (init_coders(&data, dongles) == -1)
		return (-1);
	if (init_mutexes(&data) == -1)
		return (-1);
	data.start_time = get_time();
	i = 0;
	while (i < data.num_coders)
	{
		data.coders[i].last_compile_start = data.start_time;
		i++;
	}
	if (create_threads(&data) == -1)
	{
		cleanup(&data);
		return (-1);
	}
	wait_for_threads(&data);
	cleanup(&data);
    return (0);
}

    // argc check
    // parser
    // init_dongles
    // init_coders
    // init_mutexes
    // set start_time
    // seed last_compile_start for all coders
    // create_threads
    // wait_for_threads
    // cleanup
    // return