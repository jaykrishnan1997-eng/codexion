/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 13:18:46 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/11 10:09:42 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <string.h>

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
	t_heap_node	swap;

	// add the new node at the end then sort them from min to max
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

t_heap_node	heap_extract_min(t_heap *heap, char *scheduler) {
	int			i;
	int			left;
	int			right;
	int			smallest;
	t_heap_node	min;
	t_heap_node	temp;
	
	min = heap->nodes[0];
	heap->size--;
	heap->nodes[0] = heap->nodes[heap->size];
	i = 0;
	while (1) {
		left = 2 * i + 1;
		right = 2 * i + 2;
		smallest = i;
		
		if (left < heap->size && is_smaller(heap->nodes[left], heap->nodes[smallest], scheduler))
			smallest = left;
		if (right < heap->size && is_smaller(heap->nodes[right], heap->nodes[smallest], scheduler))
			smallest = right;
		if (smallest == i)
			break;
		temp = heap->nodes[i];
		heap->nodes[i] = heap->nodes[smallest];
		heap->nodes[smallest] = temp;
		i++;
	}
	return (min);
}
