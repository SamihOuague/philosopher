/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/26 23:25:02 by souaguen          #+#    #+#             */
/*   Updated: 2024/01/30 20:17:18 by souaguen         ###   ########.fr       */
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
	pthread_t		thread;	
	pthread_mutex_t		*locked;
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
	int		i;
	t_fork		*forks;
	unsigned long		start;
	
	self = arg;
	think = 1;
	forks = (*self).forks;
	next_fork = (*self).id % (*self).n_forks;
	current_fork = (*self).id - 1;
	start = get_timestamp_ms();
	i = 0;
	while (i < 10 && (get_timestamp_ms() - start) < (*self).time_to_die)
	{
		pthread_mutex_lock((*self).locked);
		if (*(*self).started == 0)
		{
			pthread_mutex_unlock((*self).locked);
			return (NULL);
		}
		else if (forks[current_fork].free_fork == 0 && forks[next_fork].free_fork == 0)
		{
			printf("%015ld %3d has taken a fork\n", get_timestamp_ms() - (*self).start_at, (*self).id);	
			(*self).forks[current_fork].free_fork = 1;
			printf("%015ld %3d has taken a fork\n", get_timestamp_ms() - (*self).start_at, (*self).id);	
			(*self).forks[next_fork].free_fork = 1;	
			printf("%015ld %3d is eating\n", get_timestamp_ms() - (*self).start_at, (*self).id);	
			pthread_mutex_unlock((*self).locked);
			i++;
			usleep((*self).time_to_eat);
			pthread_mutex_lock((*self).locked);	
			if (*(*self).started == 0)
			{
				pthread_mutex_unlock((*self).locked);
				return (NULL);
			}
			(*self).forks[current_fork].free_fork = 0;
			(*self).forks[next_fork].free_fork = 0;
			start = get_timestamp_ms();
			printf("%015ld %3d is sleeping\n", get_timestamp_ms() - (*self).start_at, (*self).id);			
			think = 1;
			pthread_mutex_unlock((*self).locked);
			usleep((*self).time_to_sleep);
		}
		else
		{
			if (*(*self).started == 0)
			{
				pthread_mutex_unlock((*self).locked);
				return (NULL);
			}
			if (think == 1)
			{
				printf("%015ld %3d is thinking\n", get_timestamp_ms() - (*self).start_at, (*self).id);
				think = 0;
			}
			pthread_mutex_unlock((*self).locked);
		}
		usleep(1);
	}
	pthread_mutex_lock((*self).locked);
	if (*(*self).started == 0)
	{
		pthread_mutex_unlock((*self).locked);
		return (NULL);
	}
	if (i != 10)
	{
		printf("%015ld %3d died\n", get_timestamp_ms() - (*self).start_at, (*self).id);
		*(*self).started = 0;
	}
	pthread_mutex_unlock((*self).locked);
	return (NULL);
}

int	main(int argc, char **argv)
{
	t_thread_info	philo[5];
	pthread_mutex_t	mutex;
	t_fork			*forks;
	int			i;
	int			*started;
	unsigned long		start_at;

	i = -1;
	forks = malloc(sizeof(t_fork) * 5);
	if (forks == NULL)
		return (1);
	while ((++i) < 5)
		forks[i].free_fork = 0;
	started = malloc(sizeof(int));
	if (started == NULL)
		return (2);
	*started = 1;
	i = -1;
	pthread_mutex_init(&mutex, NULL);
	start_at = get_timestamp_ms();
	while ((++i) < 5)
	{
		philo[i].id = i + 1;
		philo[i].n_forks = 5;
		philo[i].forks = forks;
		philo[i].time_to_eat = 100;
		philo[i].time_to_die = 1000;
		philo[i].time_to_sleep = 500;
		philo[i].start_at = start_at;
		philo[i].locked = &mutex;
		philo[i].started = started;
		pthread_create(&philo[i].thread, NULL, &start_routine, &philo[i]);
	}
	//*started = 1;
	i = -1;
	while ((++i) < 5)
		pthread_join(philo[i].thread, NULL);
	pthread_mutex_destroy(&mutex);
	free(forks);
	free(started);
	return (0);
}
