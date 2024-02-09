/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 19:55:00 by  souaguen         #+#    #+#             */
/*   Updated: 2024/02/09 19:52:42 by souaguen         ###   ########.fr       */
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

typedef struct s_mutex_event
{
	pthread_mutex_t	eat_lock;
	pthread_mutex_t die_lock;
	pthread_mutex_t sleep_lock;	
}	t_mutex_event;

typedef struct s_mutex_status
{
	int	eat_error;
	int	die_error;
	int	sleep_error;
	int	fork_error;
}	t_mutex_status;

typedef struct s_thread_info
{
	unsigned int	id;
	pthread_t	thread;	
	unsigned int	time_to_sleep;
	unsigned int	time_to_eat;
	unsigned int	time_to_die;
	unsigned int	last_time_eat;
	unsigned int	n_fork;
	unsigned long	started_at;
	unsigned int	n_time_eat;
	t_mutex_event	*event_mutex;
	t_fork		*forks;
}	t_thread_info;
