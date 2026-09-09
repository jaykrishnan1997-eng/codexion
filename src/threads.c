/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/09 13:40:25 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/09 14:14:45 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

int     create_threads(t_data *data)
{
    n = data->num_coders
    pthread_t th[n];
    pthread_t mth;
    
    for (int i = 0; i < n; i++) {
        if (pthread_create(&th[i], NULL, &, NULL) != 0) 
            perror("Failed to create thread");
    }
    if (pthread_create(&mth, NULL, &, NULL) != 0) 
        perror("Failed to create thread");
}

void    wait_for_threads(t_data *data);
{
}
