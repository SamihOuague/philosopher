/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 19:55:00 by  souaguen         #+#    #+#             */
/*   Updated: 2024/02/07 19:57:56 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef struct s_fork
{
	pthread_mutex_t	mut;
	int		free_fork;
}	t_fork;
