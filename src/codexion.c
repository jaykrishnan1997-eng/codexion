/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 15:51:15 by jay-k             #+#    #+#             */
/*   Updated: 2026/09/04 12:16:49 by jkrishna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>


int main(int argc, char **argv){

    if (argc < 2)
        {
            printf("Correct usage is:\n")
            printf("./codexion.c nc tb tc td tr ncr dc sch")
            return -1;
`       }
            
    else
        {
            t_data *data; 
            data = parser(argv);   
        }
    
}
