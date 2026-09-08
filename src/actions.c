/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 11:03:36 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/08 14:37:15 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// 1. lock the mutex
// 2. then insert
// 3. then wait (still holding the lock)
// 4. then extract
// 5. then unlock

static void	take_one_dongle(t_coder *coder, t_dongle *dongle) {
	
	t_heap_node request;
	
	request.coder = coder;
	pthread_mutex_lock(&coder->data->sequence_mutex);
	request.sequence = coder->data->next_sequence++;
	pthread_mutex_unlock(&coder->data->sequence_mutex);
	request.deadline = coder->last_compile_start + coder->data->time_to_burnout;
	
	pthread_mutex_lock(&dongle->mutex);
	heap_insert(&dongle->request_heap, request, coder->data->scheduler);
	// is it my turn by all three condition (two_ish!)
	while (dongle->in_use
		|| get_time() < dongle->available_at
		|| dongle->request_heap.size == 0
		|| dongle->request_heap.nodes[0].coder != coder)
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
		// ok my turn, proceed
		// the second check is extra in case the heap is empty and 
		// i am trying to see whats at 0th position. So
		// basically 2 and 3 is same 

	heap_extract_min(&dongle->request_heap, coder->data->scheduler);
	// remove self from the heap now
	dongle->in_use = 1;
	pthread_mutex_unlock(&dongle->mutex);
}

void	take_dongles(t_coder *coder) {
	if (coder->left_dongle->dongle_id < coder->right_dongle->dongle_id)
	{
		take_one_dongle(coder, coder->left_dongle);
		log_state(coder, "has taken a dongle");
		take_one_dongle(coder, coder->right_dongle);
		log_state(coder, "has taken a dongle");
	}
	else
	{
		take_one_dongle(coder, coder->right_dongle);
		log_state(coder, "has taken a dongle");
		take_one_dongle(coder, coder->left_dongle);
		log_state(coder, "has taken a dongle");
	}
}

void	release_dongles(t_coder *coder) {
	if (coder->left_dongle->dongle_id < coder->right_dongle->dongle_id)
	{
		take_one_dongle(coder, coder->left_dongle);
		log_state(coder, "has taken a dongle");
		take_one_dongle(coder, coder->right_dongle);
		log_state(coder, "has taken a dongle");
	}
	else
	{
		take_one_dongle(coder, coder->right_dongle);
		log_state(coder, "has taken a dongle");
		take_one_dongle(coder, coder->left_dongle);
		log_state(coder, "has taken a dongle");
	}
}
