/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jay-k <jay-k@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 11:19:56 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/15 18:41:23 by jay-k            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_valid_number(const char *str)
{
	int	i;

	i = 0;
	if (str[i] == '\0')
		return (0);
	if (str[i] == '+')
		i++;
	if (str[i] == '\0')
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int	validate_numbers(int argc, char **argv)
{
	int	i;

	i = 1;
	if (argc != 9)
		return (-1);
	while (i <= 7)
	{
		if (!is_valid_number(argv[i]))
		{
			printf("Argument %d must be a positive integer\n", i);
			return (-1);
		}
		if (atoi(argv[i]) <= 0 && (i != 7))
		{
			printf("Argument %d must be greater than zero\n", i);
			return (-1);
		}
		if (atoi(argv[i]) < 0 && (i == 7))
		{
			printf("Argument %d must be greater than or equal to zero\n", i);
			return (-1);
		}
		i++;
	}
	return (0);
}

int	parser(int argc, char **argv, t_data *data)
{
	if (validate_numbers(argc, argv) == -1)
		return (-1);
	if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
	{
		printf("Scheduler must be exactly 'fifo' or 'edf'\n");
		return (-1);
	}
	data->num_coders = atoi(argv[1]);
	data->time_to_burnout = atoi(argv[2]);
	data->time_to_compile = atoi(argv[3]);
	data->time_to_debug = atoi(argv[4]);
	data->time_to_refractor = atoi(argv[5]);
	data->required_compiles = atoi(argv[6]);
	data->dongle_cooldown = atoi(argv[7]);
	data->scheduler = argv[8];
	return (0);
}

t_dongle	*init_dongles(t_data *data)
{
	int			d;
	t_dongle	*dongles;

	dongles = malloc(sizeof(t_dongle) * data->num_coders);
	if (!dongles)
		return (NULL);
	d = 0;
	while (d < data->num_coders)
	{
		dongles[d].dongle_id = d;
		pthread_mutex_init(&dongles[d].mutex, NULL);
		pthread_cond_init(&dongles[d].cond, NULL);
		dongles[d].in_use = 0;
		dongles[d].available_at = 0;
		dongles[d].request_heap.nodes = malloc(
				sizeof(t_heap_node) * data->num_coders);
		if (!dongles[d].request_heap.nodes)
			return (NULL);
		dongles[d].request_heap.size = 0;
		dongles[d].request_heap.capacity = data->num_coders;
		d++;
	}
	return (dongles);
}

int	init_coders(t_data *data, t_dongle *dongles)
{
	int	c;

	data->coders = malloc(sizeof(t_coder) * data->num_coders);
	if (!data->coders)
		return (-1);
	c = 0;
	while (c < data->num_coders)
	{
		data->coders[c].coder_id = c;
		data->coders[c].no_of_compiles = 0;
		data->coders[c].left_dongle = &dongles[c];
		data->coders[c].right_dongle = &dongles[(c + 1) % data->num_coders];
		data->coders[c].data = data;
		data->coders[c].last_compile_start = 0;
		pthread_mutex_init(&data->coders[c].state_mutex, NULL);
		c++;
	}
	return (0);
}
