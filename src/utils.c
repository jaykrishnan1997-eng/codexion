/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jay-k <jay-k@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 14:47:54 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/10 20:26:47 by jay-k            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>

bool    checkin(char c, char *str) {

    int i;
    int length;

	i = 0;
    length = strlen(str);
    while (i < length) {

        if (c == str[i])
            return (true);
        i++;
    }
    return (false);
}

long	get_time(void) {
	// struct timeval {
	// 	time_t		tv_sec;	// second
	// 	suseconds_t	tv_usec;// microseconds
	// }; pre-defined in sys/time.h
	// int gettimeofday(struct timeval *tv, struct timezone *tz);
	
	long time_in_ms;
	struct timeval	tv;
	
	gettimeofday(&tv, NULL);
	time_in_ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	return (time_in_ms);
}

void	log_state(t_coder *coder, char *message)
{
	printf("%d %d %s", get_time(), coder->coder_id, message);
}
