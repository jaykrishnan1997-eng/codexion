/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jay-k <jay-k@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 13:18:46 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/07 20:52:37 by jay-k            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

static int is_smaller(t_heap_node a, t_heap_node b, char *scheduler)
{
	if (strcmp(scheduler, "fifo") == 0)
		return (a.sequence < b.sequence);
	else
		return (a.deadline < b.deadline);
}

void	heap_insert(t_heap *heap, t_heap_node new_node, char *scheduler) {
    int			i;
	int 		parent;
	t_heap_node	temp;

	heap->nodes[heap->size] = new_node;
	i = heap->size;
	heap->size++;

	while (i > 0) {
		parent = (i - 1) / 2;
		if (is_smaller(heap->nodes[i], heap->nodes[parent], scheduler))
		{
			swap = heap->nodes[i];
			heap->nodes[i] = heap->nodes[parent];
			heap->nodes[parent] = swap;
			i = parent;
		}
		else
			break;
	}
}

void	heap_extract_min() {
    
}
