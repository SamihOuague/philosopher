/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/26 23:25:02 by souaguen          #+#    #+#             */
/*   Updated: 2024/01/29 17:33:17 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_fork
{
	pthread_mutex_t	mut;
	int		free_fork;
}	t_fork;

typedef struct s_thread_info 
{
	pthread_t	thread;
	unsigned long		start_at;
	unsigned int		id;
	unsigned int		n_forks;
	unsigned int		time_to_die;
	unsigned int		time_to_sleep;
	unsigned int		time_to_eat;
	int				*started;
	t_fork		*forks;
}	t_thread_info;

unsigned long	get_timestamp_ms()
{
	struct timeval	tv;
	unsigned long	ms_timestamp;

	gettimeofday(&tv, NULL);
	ms_timestamp = (tv.tv_sec * 1000000) + tv.tv_usec;
	return (ms_timestamp);
}

void	*start_routine(void *arg)
{
	t_thread_info	*self;
	int		next_fork;
	int		current_fork;
	int		n_fork_taken;
	int		think;
	t_fork		*forks;
	unsigned long		start;
	
	self = arg;
	think = 0;
	forks = (*self).forks;
	while (*(*self).started == 0)
		usleep(1);
	next_fork = (*self).id % (*self).n_forks;
	current_fork = (*self).id - 1;
	start = get_timestamp_ms();
	while ((get_timestamp_ms() - start) < (*self).time_to_die)
	{
		if (forks[current_fork].free_fork == 0 && forks[next_fork].free_fork == 0)
		{
			forks[current_fork].free_fork = 1;
			pthread_mutex_lock(&forks[current_fork].mut);
			printf("%015ld %3d has taken a fork\n", get_timestamp_ms() - (*self).start_at, (*self).id);
			forks[next_fork].free_fork = 1;
			pthread_mutex_lock(&forks[next_fork].mut);	
			printf("%015ld %3d has taken a fork\n", get_timestamp_ms() - (*self).start_at, (*self).id);
			printf("%015ld %3d is eating\n", get_timestamp_ms() - (*self).start_at, (*self).id);
			usleep((*self).time_to_eat);
			start = get_timestamp_ms();
			pthread_mutex_unlock(&forks[current_fork].mut);	
			forks[current_fork].free_fork = 0;
			pthread_mutex_unlock(&forks[next_fork].mut);	
			forks[next_fork].free_fork = 0;
			printf("%015ld %3d is sleeping\n", get_timestamp_ms() - (*self).start_at, (*self).id);
			usleep((*self).time_to_sleep);
			think = 0;
		}
		else if (think == 0)
		{
			think = 1;
			printf("%015ld %3d is thinking\n", get_timestamp_ms() - (*self).start_at, (*self).id);
		}
		usleep(10);
	}
	printf("%015ld %3d died\n", get_timestamp_ms() - (*self).start_at, (*self).id);
	return (NULL);
}

int	main(int argc, char **argv)
{
	t_thread_info	philo[4];
	t_fork			*forks;
	int			i;
	int			*started;

	i = -1;
	forks = malloc(sizeof(t_fork) * 4);
	if (forks == NULL)
		return (1);
	while ((++i) < 4)
	{
		forks[i].free_fork = 0;
		pthread_mutex_init(&forks[i].mut, NULL);
	}
	started = malloc(sizeof(int));
	if (started == NULL)
		return (2);
	*started = 0;
	i = -1;
	while ((++i) < 4)
	{
		philo[i].id = i + 1;
		philo[i].n_forks = 4;
		philo[i].forks = forks;
		philo[i].time_to_eat = 1000;
		philo[i].time_to_die = 1000;
		philo[i].time_to_sleep = 1000;
		philo[i].start_at = get_timestamp_ms();
		philo[i].started = started;
		pthread_create(&philo[i].thread, NULL, &start_routine, &philo[i]);
	}
	*started = 1;
	i = -1;
	while ((++i) < 4)
		pthread_join(philo[i].thread, NULL);
	return (0);
}
