/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 15:51:15 by jay-k             #+#    #+#             */
/*   Updated: 2026/09/15 12:53:27 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_data(t_data *data)
{
	t_dongle	*dongles;
	int			i;

	i = 0;
	data->simulation_over = 0;
	dongles = init_dongles(data);
	if (!dongles)
		return (-1);
	data->dongles = dongles;
	if (init_coders(data, dongles) == -1)
		return (-1);
	if (init_mutexes(data) == -1)
		return (-1);
	data->start_time = get_time();
	while (i < data->num_coders)
	{
		data->coders[i].last_compile_start = data->start_time;
		i++;
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_data	data;

	if (argc != 9)
	{
		printf("Correct usage is:\n\n");
		printf(
			"./codexion number_of_coders time_to_burnout"
			" time_to_compile time_to_debug time_to_refactor"
			" number_of_compiles_required dongle_cooldown scheduler\n\n");
		printf("eg: ./codexion 6 3000 200 100 100 2 50 edf\n");
		return (-1);
	}
	if (parser(argc, argv, &data) == -1)
		return (-1);
	if (init_data(&data) == -1)
		return (-1);
	if (create_threads(&data) == -1)
	{
		cleanup(&data);
		return (-1);
	}
	wait_for_threads(&data);
	cleanup(&data);
	return (0);
}
