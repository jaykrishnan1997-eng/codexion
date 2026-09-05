/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 14:47:54 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/05 11:10:29 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <stdbool.h>

bool    checkin(char c, char *str) {

    int i;
    int length;

    length = strlen(str);
    while (i < length) {

        if (c == str[i])
            return (true);
        i++;
    }
    return (false);
}

