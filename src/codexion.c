/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 15:51:15 by jay-k             #+#    #+#             */
/*   Updated: 2026/09/07 11:21:27 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>


int main(int argc, char **argv){

    t_data		data;
	t_dongle	*dongles;

    if (argc != 9)
    {
		printf("Correct usage is:\n");
		printf("./codexion.c nc tb tc td tr ncr dc sch");
		return (-1);
	}
    if (parser(argc, argv, &data) == -1)
        return (-1);
	dongles = init_dongles(&data);
	if (!dongles)
		return (-1);
	data.dongles = dongles;
	if (init_coders(&data, dongles) == -1)
		return (-1);
	if (init_mutexes(&data) == -1)
		return (-1);
	data.start_time = get_time();
	
	
    // create thread, sun simulation, cleanups etc
    return (0);
}

// Given where you are, the fastest realistic path forward: 
// write the heap + FIFO/EDF comparators next, test standalone
//  with fake data, then immediately wire it into dongle acquisition
//  — that's the biggest remaining chunk of new logic, and getting it
//  right early prevents rework everywhere else. Want to jump straight into that?