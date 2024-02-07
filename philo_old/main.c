/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/26 23:25:02 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/07 04:32:52 by souaguen         ###   ########.fr       */
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
	unsigned int		n_time_eat;
	int				*started;
	t_fork		*forks;
}	t_thread_info;

int	ft_atoi(char *str)
{
	int	n;
	int	i;
	int	sign;

	n = 0;
	i = 0;
	sign = 1;
	if (*(str) == '-')
	{
		sign *= -1;
		i++;
	}
	while (*(str + i) >= '0' && *(str + i) <= '9')
		n = (n * 10) + (*(str + (i++)) - '0');
	return (n * sign);
}

unsigned long	get_timestamp_ms()
{
	struct timeval	tv;
	unsigned long	ms_timestamp;

	gettimeofday(&tv, NULL);
	ms_timestamp = (tv.tv_sec * 1000000) + tv.tv_usec;
	return (ms_timestamp);
}

void	mut_print(char *format, t_thread_info *self)
{
	pthread_mutex_lock((*self).locked);
	printf(format, get_timestamp_ms() - (*self).start_at, (*self).id);
	pthread_mutex_unlock((*self).locked);
}

int	are_forks_free(t_fork *forks, t_thread_info *self)
{
	int	c;
	int	next_fork;
	int	current_fork;

	c = 1;
	next_fork = (*self).id % (*self).n_forks;
	current_fork = (*self).id - 1;
	pthread_mutex_lock(&forks[current_fork].mut);
	if (forks[current_fork].free_fork != 0)
		c = 0;
	pthread_mutex_unlock(&forks[current_fork].mut);
	pthread_mutex_lock(&forks[next_fork].mut);
	if (forks[next_fork].free_fork != 0)
		c = 0;
	pthread_mutex_unlock(&forks[next_fork].mut);
	return (c);
}

int	take_a_fork(int fork_index, t_thread_info *self)
{
	int	i;

	i = 0;
	pthread_mutex_lock(&(*self).forks[fork_index].mut);
	if (!(*self).forks[fork_index].free_fork)
	{
		(*self).forks[fork_index].free_fork = 1;
		i = 1;
	}
	pthread_mutex_unlock(&(*self).forks[fork_index].mut);
	return (i);
}

void	give_back_fork(int fork_index, t_thread_info *self)
{
	pthread_mutex_lock(&(*self).forks[fork_index].mut);
	(*self).forks[fork_index].free_fork = 0;
	pthread_mutex_unlock(&(*self).forks[fork_index].mut);
}

void	*start_routine(void *arg)
{
	t_thread_info	*self;
	int		next_fork;
	int		current_fork;
	int		think;	
	unsigned int		i;
	t_fork		*forks;
	unsigned long		start;
	int		eating;

	i = 0;	
	think = 1;
	self = arg;
	eating = 0;
	forks = (*self).forks;
	start = (*self).start_at;
	next_fork = (*self).id % (*self).n_forks;
	current_fork = (*self).id - 1;
	while ((*self).n_forks >= 1 && (get_timestamp_ms() - start) < (*self).time_to_die)
	{
		pthread_mutex_lock((*self).locked);
		if (*(*self).started == 0)
		{
			pthread_mutex_unlock((*self).locked);
			return (NULL);
		}
		pthread_mutex_unlock((*self).locked);
		if (are_forks_free(forks, self))
		{
			if (take_a_fork(current_fork, self))
				eating = take_a_fork(next_fork, self);
			if (eating)
			{
				mut_print("%ld %d has taken a fork\n", self);
				mut_print("%ld %d has taken a fork\n", self);
				think = 0;
			}
			else
			{
				think = 1;
				give_back_fork(current_fork, self);
			}
		}	
		if (think == 1)
		{
			think = 0;
			usleep(1);
			pthread_mutex_lock((*self).locked);
			if (*(*self).started == 0)
			{
				pthread_mutex_unlock((*self).locked);
				return (NULL);
			}
			pthread_mutex_unlock((*self).locked);
			mut_print("%ld %d is thinking\n", self);
		}
		else if (eating)
		{
			usleep(1);
			pthread_mutex_lock((*self).locked);	
			if (*(*self).started == 0)
			{
				pthread_mutex_unlock((*self).locked);
				return (NULL);
			}
			pthread_mutex_unlock((*self).locked);
			mut_print("%ld %d is eating\n", self);
			if ((*self).n_time_eat != -1)	
				i++;
			usleep((*self).time_to_eat);
			start = get_timestamp_ms();
			give_back_fork(current_fork, self);
			give_back_fork(next_fork, self);
			if ((*self).n_time_eat != -1 && i >= (*self).n_time_eat)	
				break ;
			pthread_mutex_lock((*self).locked);
			usleep(1);
			if (*(*self).started == 0)
			{
				pthread_mutex_unlock((*self).locked);
				return (NULL);
			}
			pthread_mutex_unlock((*self).locked);
			mut_print("%ld %d is sleeping\n", self);
			usleep((*self).time_to_sleep);		
			eating = 0;
		}
		usleep(1);
	}
	if (i != (*self).n_time_eat)
	{	
		pthread_mutex_lock((*self).locked);	
		if (*(*self).started == 0)
		{
			pthread_mutex_unlock((*self).locked);
			return (NULL);
		}	
		*(*self).started = 0;
		pthread_mutex_unlock((*self).locked);
		mut_print("%ld %d died\n", self);
	}
	//printf("%d died\n", (*self).id);
	/*while ((*self).n_forks >= 1 && (i < (*self).n_time_eat) && (get_timestamp_ms() - start) < (*self).time_to_die)
	{
		pthread_mutex_lock((*self).locked);
		if (*(*self).started == 0)
		{
			pthread_mutex_unlock((*self).locked);
			return (NULL);
		}
		else if (forks[current_fork].free_fork == 0 && forks[next_fork].free_fork == 0)
		{
			printf("%ld %d has taken a fork\n", get_timestamp_ms() - (*self).start_at, (*self).id);	
			(*self).forks[current_fork].free_fork = 1;
			if ((*self).n_forks == 1)
			{
				pthread_mutex_unlock((*self).locked);
				continue ;
			}
			printf("%ld %d has taken a fork\n", get_timestamp_ms() - (*self).start_at, (*self).id);	
			(*self).forks[next_fork].free_fork = 1;	
			printf("%ld %d is eating\n", get_timestamp_ms() - (*self).start_at, (*self).id);	
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
			printf("%ld %d is sleeping\n", get_timestamp_ms() - (*self).start_at, (*self).id);			
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
				printf("%ld %d is thinking\n", get_timestamp_ms() - (*self).start_at, (*self).id);
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
	if (i != (*self).n_time_eat)
	{
		printf("%ld %d died\n", get_timestamp_ms() - (*self).start_at, (*self).id);
		*(*self).started = 0;
	}

	pthread_mutex_unlock((*self).locked);*/
	return (NULL);
}

int	main(int argc, char **argv)
{
	t_thread_info	*philo;
	pthread_mutex_t	mutex;
	t_fork			*forks;
	int			i;
	int			*started;
	unsigned long		start_at;
	
	if (!(argc == 5 || argc == 6))
		return (1);
	i = -1;
	forks = malloc(sizeof(t_fork) * ft_atoi(argv[1]));
	philo = malloc(sizeof(t_thread_info) * ft_atoi(argv[1]));
	if (forks == NULL)
		return (1);
	while ((++i) < ft_atoi(argv[1]))
	{
		forks[i].free_fork = 0;
		pthread_mutex_init(&forks[i].mut, NULL);
	}
	started = malloc(sizeof(int));
	if (started == NULL)
		return (2);
	*started = 1;
	i = -1;
	pthread_mutex_init(&mutex, NULL);
	start_at = get_timestamp_ms();
	while ((++i) < ft_atoi(argv[1]))
	{
		philo[i].id = i + 1;
		philo[i].n_forks = ft_atoi(argv[1]);
		philo[i].forks = forks;
		philo[i].time_to_eat = ft_atoi(argv[3]);
		philo[i].time_to_die = ft_atoi(argv[2]);
		philo[i].time_to_sleep = ft_atoi(argv[4]);
		philo[i].start_at = start_at;
		philo[i].locked = &mutex;
		philo[i].started = started;
		philo[i].n_time_eat = -1;
		if (argc == 6)
			philo[i].n_time_eat = ft_atoi(argv[5]);
		pthread_create(&philo[i].thread, NULL, &start_routine, &philo[i]);
	}
	i = -1;
	while ((++i) < ft_atoi(argv[1]))
	{
		pthread_join(philo[i].thread, NULL);
		pthread_mutex_destroy(&forks[i].mut);
	}
	pthread_mutex_destroy(&mutex);
	free(forks);
	free(started);
	free(philo);
	return (0);
}
