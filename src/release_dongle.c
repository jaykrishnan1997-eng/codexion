/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   release_dongle.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 08:45:21 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/15 10:56:40 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	release_one_dongles(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->available_at = get_time() + coder->data->dongle_cooldown;
	dongle->in_use = 0;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

void	release_dongles(t_coder *coder)
{
	if (coder->left_dongle == coder->right_dongle)
	{
		release_one_dongles(coder, coder->left_dongle);
		return ;
	}
	release_one_dongles(coder, coder->left_dongle);
	release_one_dongles(coder, coder->right_dongle);
}
