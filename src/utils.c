/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 14:47:54 by jkrishna          #+#    #+#             */
/*   Updated: 2026/09/04 15:34:18 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

bool    checkin(char c, char *str) {

    int i;
    int length;

    length = strlen(str);
    for (i = 0; i < length; i++) {
        if (c == str[i])
            return true
    }
    return false
}

