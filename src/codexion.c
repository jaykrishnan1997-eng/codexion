/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 15:51:15 by jay-k             #+#    #+#             */
/*   Updated: 2026/09/05 11:22:59 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>


int main(int argc, char **argv){

    t_data data;
    if (argc != 9)
        {
            printf("Correct usage is:\n")
            printf("./codexion.c nc tb tc td tr ncr dc sch")
            return (-1);
`       }
    if (parser(argc, argv, &data) == -1)
        return (-1);
    // create thread, sun simulation, cleanups etc
    return (0);
    
}
