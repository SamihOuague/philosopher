/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 00:48:28 by  souaguen         #+#    #+#             */
/*   Updated: 2024/02/13 08:43:06 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	precision_sleep(unsigned int *time_to_sleep)
{
	usleep(*time_to_sleep * 1000);
}

int	locked_printf(char *format, t_thread_info *self)
{
	int	dead;

	dead = 0;
	pthread_mutex_lock((*self).locked);
	if (*(*self).deadbeef == 1)
	{
		pthread_mutex_unlock((*self).locked);
		return (0);
	}
	printf(format, get_timestamp_ms() - *(*self).started_at, (*self).id);
	pthread_mutex_unlock((*self).locked);
	return (1);
}

void	take_forks(t_thread_info *self)
{
	t_fork		*forks;
	unsigned long	checkpoint;
	int		current_fork;
	int		next_fork;

	forks = (*self).forks;
	current_fork = (*self).id - 1;
	next_fork = (*self).id % (*self).n_fork;
	if ((*self).id % 2 == 0)
	{
		current_fork = next_fork;
		next_fork = (*self).id - 1;
	}
	checkpoint = get_timestamp_ms();
	pthread_mutex_lock(&forks[current_fork].mut);
	locked_printf("%ld %d has taken a fork\n", self);
	pthread_mutex_lock(&forks[next_fork].mut);
	locked_printf("%ld %d has taken a fork\n", self);
}

void	start_eating(t_thread_info *self)
{
	t_fork		*forks;
	int		current_fork;
	int		next_fork;

	forks = (*self).forks;
	current_fork = (*self).id - 1;
	next_fork = (*self).id % (*self).n_fork;
	locked_printf("%ld %d is eating\n", self);
	(*self).last_meal = get_timestamp_ms();
	precision_sleep(&(*self).time_to_eat);	
	(*self).last_meal = get_timestamp_ms();
	pthread_mutex_unlock(&forks[next_fork].mut);
	pthread_mutex_unlock(&forks[current_fork].mut);
}

void	start_sleeping(t_thread_info *self)
{
	int	time_to_sleep;

	time_to_sleep = (*self).time_to_sleep;
	locked_printf("%ld %d is sleeping\n", self);
	precision_sleep(&time_to_sleep);
}

void	philo_dead(t_thread_info *self)
{

	pthread_mutex_lock((*self).locked);
	if (*(*self).deadbeef == 1)
	{
		pthread_mutex_unlock((*self).locked);
		return ;
	}
	*(*self).deadbeef = 1;
	printf("%ld %d died\n", get_timestamp_ms() - *(*self).started_at, (*self).id);
	pthread_mutex_unlock((*self).locked);
}

int	dead_philo(t_thread_info *philo, pthread_mutex_t *locked)
{
	int	i;
	int	n_philo;

	i = 0;
	pthread_mutex_lock(locked);	
	n_philo = philo[0].n_fork;
	while (i < n_philo)
	{
		if ((get_timestamp_ms() - philo[i].last_meal) >= philo[i].time_to_die)
		{
			pthread_mutex_unlock(locked);
			return (i);
		}
		i++;
	}
	pthread_mutex_unlock(locked);
	return(-1);
}

void	*dead_monitor(void *arg)
{
	t_thread_info	*philos;
	pthread_mutex_t	*meal_lock;
	pthread_mutex_t	*locked;

	philos = arg;
	locked = (*philos).locked;
	meal_lock = (*philos).meal_lock;
	while (1)
	{
		if (dead_philo(philos, meal_lock) != -1)
			
		usleep(10000);
	}
	return (NULL);
}

void	*start_routine(void *arg)
{
	t_thread_info *self;
	long	checkpoint;
	unsigned long	start;
	int		i;
	t_fork		*forks;

	i = 0;
	self = arg;
	forks = (*self).forks;
	start = *(*self).started_at;
	if ((*self).n_fork == 1)
	{
		pthread_mutex_lock(&forks[(*self).id - 1].mut);	
		locked_printf("%ld %d is thinking\n", self);
		locked_printf("%ld %d has taken a fork\n", self);
		precision_sleep(&(*self).time_to_die);
		pthread_mutex_unlock(&forks[(*self).id - 1].mut);
		philo_dead(self);
		return (NULL);
	}
	(*self).last_meal = get_timestamp_ms();
	pthread_mutex_lock((*self).locked);
	while (i != (*self).n_time_eat && *(*self).deadbeef != 1)
	{
		pthread_mutex_unlock((*self).locked);
		locked_printf("%ld %d is thinking\n", self);
		take_forks(self);
		start_eating(self);
		i = i + 1;
		start_sleeping(self);
		pthread_mutex_lock((*self).locked);
	}
	pthread_mutex_unlock((*self).locked);
	return (NULL);
}

void	run(t_thread_info *philo, int n_philo)
{
	int		i;
	unsigned long	start;
	pthread_mutex_t	locked;
	pthread_mutex_t	meal_lock;	
	int		deadbeef;
	int		started;
	pthread_t	monitor;

	i = -1;
	deadbeef = 0;
	started = 0;
	pthread_mutex_init(&locked, NULL);
	pthread_mutex_init(&meal_lock, NULL);
	start = get_timestamp_ms();
	while ((++i) < n_philo)
	{
		philo[i].meal_lock = &meal_lock;
		philo[i].last_meal = get_timestamp_ms();
		philo[i].started_at = &start;
		philo[i].locked = &locked;
		philo[i].deadbeef = &deadbeef;
		philo[i].started = &started;
		pthread_create(&philo[i].thread, NULL, &start_routine, &philo[i]);
	}
	pthread_create(&monitor, NULL, &dead_monitor, philo);	
	i = -1;
	while ((++i) < n_philo)
		pthread_join(philo[i].thread, NULL);
	pthread_join(monitor, NULL);
	pthread_mutex_destroy(&locked);
	pthread_mutex_destroy(&meal_lock);
}

int	main(int argc, char **argv)
{
	int		n_philo;
	t_thread_info	*philo;

	if (!(argc == 5 || argc == 6))
		return (1);
	n_philo = ft_atoi(argv[1]);
	philo = philo_info_init(argc, argv);
	if (philo == NULL)
		return (1);
	run(philo, n_philo);
	free_forks((*philo).forks, n_philo);
	free(philo);
	return (0);
}
