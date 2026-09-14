/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   take_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 11:03:36 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/14 09:10:15 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	wait_for_dongle(t_coder *coder, t_dongle *dongle)
{
	struct timespec	ts;

	while (dongle->in_use
		|| get_time() < dongle->available_at
		|| dongle->request_heap.size == 0
		|| dongle->request_heap.nodes[0].coder != coder)
	{
		if (is_simulation_over(coder->data))
			return (-1);
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_nsec += 5000000;
		if (ts.tv_nsec >= 1000000000)
		{
			ts.tv_sec += 1;
			ts.tv_nsec -= 1000000000;
		}
		pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
	}
	return (0);
}

static int	take_one_dongle(t_coder *coder, t_dongle *dongle)
{
	t_heap_node	request;

	request.coder = coder;
	pthread_mutex_lock(&coder->data->sequence_mutex);
	request.sequence = coder->data->next_sequence++;
	pthread_mutex_unlock(&coder->data->sequence_mutex);
	pthread_mutex_lock(&coder->state_mutex);
	request.deadline = coder->last_compile_start + coder->data->time_to_burnout;
	pthread_mutex_unlock(&coder->state_mutex);
	pthread_mutex_lock(&dongle->mutex);
	heap_insert(&dongle->request_heap, request, coder->data->scheduler);
	if (wait_for_dongle(coder, dongle) == -1)
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (-1);
	}
	heap_extract_min(&dongle->request_heap, coder->data->scheduler);
	dongle->in_use = 1;
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}

static int	take_ordered_dongles(t_coder *coder)
{
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
	return (take_ordered_dongles(coder));
}
