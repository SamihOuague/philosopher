/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 19:54:41 by  souaguen         #+#    #+#             */
/*   Updated: 2024/02/09 22:42:25 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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

unsigned long   get_timestamp_ms()
{
	struct timeval  tv;
	unsigned long   ms_timestamp;

	gettimeofday(&tv, NULL);
	ms_timestamp = (tv.tv_sec * 1000000) + tv.tv_usec;
	return (ms_timestamp);
}

void	ft_putstr_fd(char *str, int fd)
{
	int	i;

	i = -1;
	while (*(str + (++i)) != '\0')
		write(fd, (str + i), 1);
}

void	*forks_init(unsigned int n_philo)
{
	int	i;
	t_fork	*forks;

	i = -1;
	forks = malloc(sizeof(t_fork) * n_philo);
	if (forks == NULL)
		return (NULL);
	while ((++i) < n_philo)
	{
		forks[i].free_fork = 0;
		if (pthread_mutex_init(&forks[i].mut, NULL) != 0)
		{
			while ((--i) >= 0)
				pthread_mutex_destroy(&forks[i].mut);
			return (NULL);
		}
	}
	return (forks);
}

void	*philo_info_init(int argc, char **argv)
{
	int		i;
	int		n_philo;
	t_thread_info	*philo;
	t_fork		*forks;

	i = -1;
	n_philo = ft_atoi(argv[1]);
	philo = malloc(sizeof(t_thread_info) * n_philo);
	forks = forks_init(n_philo);
	if (philo == NULL || forks == NULL)
		return (NULL);
	while ((++i) < n_philo)
	{
		philo[i].id = i + 1;
		philo[i].time_to_die = ft_atoi(argv[2]);
		philo[i].time_to_eat = ft_atoi(argv[3]);
		philo[i].time_to_sleep = ft_atoi(argv[4]);
		philo[i].n_time_eat = -1;
		philo[i].n_fork = n_philo;
		philo[i].forks = forks;
		if (argc == 6)
			philo[i].n_time_eat = ft_atoi(argv[5]);
	}
	return (philo);
}

int	philo_mutex_destroy(t_mutex_event *mutex)
{
	if (pthread_mutex_destroy(&(*mutex).eat_lock) != 0)
		return (1);
	else if (pthread_mutex_destroy(&(*mutex).die_lock) != 0)
		return (1);
	else if (pthread_mutex_destroy(&(*mutex).sleep_lock) != 0)
		return (1);
	return (0);
}

int	philo_mutex_init(t_mutex_event *mutex)
{
	pthread_mutex_init(&(*mutex).eat_lock, NULL);
	pthread_mutex_init(&(*mutex).die_lock, NULL);
	pthread_mutex_init(&(*mutex).sleep_lock, NULL);
	return (0);
}

void	free_forks(t_fork *forks, int n_forks)
{
	int	i;
	
	i = -1;
	while ((++i) < n_forks)
		pthread_mutex_destroy(&forks[i].mut);
	free(forks);
}

int	give_back_fork(t_fork *forks, unsigned int id)
{
	if (pthread_mutex_lock(&forks[id].mut) != 0)
		return (0);
	forks[id].free_fork = 0;
	pthread_mutex_unlock(&forks[id].mut);
	return (1);
}

int	take_a_fork(t_fork *forks, unsigned int id, unsigned int n_fork)
{
	int	current_fork;
	int	next_fork;

	current_fork = id - 1;
	next_fork = id % n_fork;	
	if (pthread_mutex_lock(&forks[current_fork].mut) != 0)
		return (0);
	//printf("%d %d - ", current_fork, next_fork);
	//printf("%d %d\n", forks[current_fork].free_fork, forks[next_fork].free_fork);
	if (forks[current_fork].free_fork == 0)
		forks[current_fork].free_fork = 1;
	else
		return (pthread_mutex_unlock(&forks[current_fork].mut));
	pthread_mutex_unlock(&forks[current_fork].mut);
	if (pthread_mutex_lock(&forks[next_fork].mut) != 0)
	{
		give_back_fork(forks, current_fork);
		return (0);
	}
	if (forks[next_fork].free_fork == 0)
		forks[next_fork].free_fork = 1;
	else
		return (pthread_mutex_unlock(&forks[next_fork].mut));
	pthread_mutex_unlock(&forks[next_fork].mut);
	return (1);
}

void	*start_routine(void *arg)
{
	unsigned long	start;
	t_thread_info	*self;
	t_mutex_event	*event_mut;
	t_fork		*forks;
	int		think;
	
	self = (t_thread_info *) arg;
	event_mut = (*self).event_mutex;
	forks = (*self).forks;
	start = (*self).started_at;
	think = 0;
	while ((get_timestamp_ms() - start) < (*self).time_to_die)
	{
		if (take_a_fork(forks, (*self).id, (*self).n_fork) == 1)
		{
			if (!pthread_mutex_lock(&(*event_mut).eat_lock))
			{
				printf("%ld %d has taken a fork\n", get_timestamp_ms() - start, (*self).id);
				printf("%ld %d is eating\n", get_timestamp_ms() - start, (*self).id);
				pthread_mutex_unlock(&(*event_mut).eat_lock);
				usleep((*self).time_to_eat);
				give_back_fork(forks, (*self).id - 1);
				give_back_fork(forks, (*self).id % (*self).n_fork);
			}	
			if (!pthread_mutex_lock(&(*event_mut).eat_lock))
			{
				printf("%ld %d is sleeping\n", get_timestamp_ms() - start, (*self).id);
				pthread_mutex_unlock(&(*event_mut).eat_lock);
				usleep((*self).time_to_sleep);
			}
			
		}
		else if (think == 1)
		{
			think = 0;
			if (!pthread_mutex_lock(&(*event_mut).eat_lock))
			{
				//printf("%ld %d is thinking\n", get_timestamp_ms() - start, (*self).id);
				pthread_mutex_unlock(&(*event_mut).eat_lock);
			}
		}
		else
			think = 1;
	}
	return (NULL);
}

int	main(int argc, char **argv)
{
	t_thread_info	*philo;
	t_mutex_event	*event_mutex;
	unsigned int	n_philo;
	unsigned int	i;
	unsigned long	start;

	if (argc < 5 || argc > 6)
	{
		ft_putstr_fd("Usage: ./philo [N philos] [time to die] [time to eat] ", 2);
		ft_putstr_fd("[time to sleep] (optional)[N time to eat]\n", 2);
		return (1);
	}	
	event_mutex = malloc(sizeof(t_mutex_event));
	if (event_mutex == NULL)
		return (1);
	i = -1;
	n_philo = ft_atoi(argv[1]);
	philo = philo_info_init(argc, argv);
	philo_mutex_init(event_mutex);
	start = get_timestamp_ms();
	while ((++i) < n_philo)
	{
		philo[i].started_at = start;
		philo[i].event_mutex = event_mutex;
		pthread_create(&philo[i].thread, NULL, &start_routine, &philo[i]);
	}
	i = -1;
	while ((++i) < n_philo)
		pthread_join(philo[i].thread, NULL);
	philo_mutex_destroy(event_mutex);
	free_forks(philo[0].forks, n_philo);
	free(philo);
	free(event_mutex);
	return (0);
}
