/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 11:03:36 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/12 09:42:31 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <time.h>

// 1. lock the mutex
// 2. then insert
// 3. then wait (still holding the lock)
// 4. then extract
// 5. then unlock


static int	take_one_dongle(t_coder *coder, t_dongle *dongle) {
	
	t_heap_node request;
	struct timespec ts;
	request.coder = coder;
	pthread_mutex_lock(&coder->data->sequence_mutex);
	request.sequence = coder->data->next_sequence++;
	pthread_mutex_unlock(&coder->data->sequence_mutex);
	pthread_mutex_lock(&coder->state_mutex);
	request.deadline = coder->last_compile_start + coder->data->time_to_burnout;
	pthread_mutex_unlock(&coder->state_mutex);
	
	pthread_mutex_lock(&dongle->mutex);
	heap_insert(&dongle->request_heap, request, coder->data->scheduler);
	// is it my turn by all three condition (two_ish!)
	while (dongle->in_use
		|| get_time() < dongle->available_at
		|| dongle->request_heap.size == 0
		|| dongle->request_heap.nodes[0].coder != coder)
		{
			if (is_simulation_over(coder->data))
			{
				pthread_mutex_unlock(&dongle->mutex);
				return (-1);
			}
			clock_gettime(CLOCK_REALTIME, &ts);
			ts.tv_nsec += 5000000;
			if (ts.tv_nsec >= 1000000000)
			{
				ts.tv_sec += 1;
				ts.tv_nsec -= 1000000000;
			}
			pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
		}
		// ok my turn, proceed
		// the second check is extra in case the heap is empty and 
		// i am trying to see whats at 0th position. So
		// basically 2 and 3 is same 

	heap_extract_min(&dongle->request_heap, coder->data->scheduler);
	// remove self from the heap now
	dongle->in_use = 1;
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}

int	take_dongles(t_coder *coder) 
{
	if (is_simulation_over(coder->data))
		return (-1);
	if (coder->left_dongle == coder->right_dongle)
	{
		if (take_one_dongle(coder, coder->left_dongle) == -1)
			return (-1);
		log_state(coder, "has taken a dongle");
		log_state(coder, "has taken a dongle");
		return (0);
	}
	if (coder->left_dongle->dongle_id < coder->right_dongle->dongle_id)
	{
		if (take_one_dongle(coder, coder->left_dongle) == -1)
			return (-1);
		log_state(coder, "has taken a dongle");
		if (take_one_dongle(coder, coder->right_dongle) == -1)
			return (-1);
		log_state(coder, "has taken a dongle");
	}
	else
	{
		if (take_one_dongle(coder, coder->right_dongle) == -1)
			return (-1);
		log_state(coder, "has taken a dongle");
		if (take_one_dongle(coder, coder->left_dongle) == -1)
			return (-1);
		log_state(coder, "has taken a dongle");
	}
	return (0);
}

static void	release_one_dongles(t_coder *coder, t_dongle *dongle) {

	pthread_mutex_lock(&dongle->mutex);
	dongle->available_at = get_time() + coder->data->dongle_cooldown;
	dongle->in_use = 0;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

void release_dongles(t_coder *coder)
{
	if (coder->left_dongle == coder->right_dongle)
	{
		release_one_dongles(coder, coder->left_dongle);
		return;
	}
	release_one_dongles(coder, coder->left_dongle);
	release_one_dongles(coder, coder->right_dongle);
}