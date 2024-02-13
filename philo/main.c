/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 00:48:28 by  souaguen         #+#    #+#             */
/*   Updated: 2024/02/13 05:20:21 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	precision_sleep(unsigned long *checkpoint, unsigned int *time_to_sleep)
{
	long	t;

	t = (long)(*time_to_sleep);// - ((long)get_timestamp_ms() - (long)(*checkpoint));
	//if (t <= 0 || t > *time_to_sleep)
	//	return ;
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

long	take_forks(t_thread_info *self)
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
	if ((get_timestamp_ms() - (*self).last_meal) > (*self).time_to_die)
		return (-1);
	pthread_mutex_lock(&forks[current_fork].mut);
	if (!locked_printf("%ld %d has taken a fork\n", self))
	{
		pthread_mutex_unlock(&forks[current_fork].mut);
		return (-1);
	}
	pthread_mutex_lock(&forks[next_fork].mut);
	if (!locked_printf("%ld %d has taken a fork\n", self))
	{
		pthread_mutex_unlock(&forks[current_fork].mut);
		pthread_mutex_unlock(&forks[next_fork].mut);
		return (-1);
	}
	return (get_timestamp_ms());
}

long	start_eating(t_thread_info *self, unsigned long *checkpoint)
{
	t_fork		*forks;
	int		current_fork;
	int		next_fork;

	forks = (*self).forks;
	current_fork = (*self).id - 1;
	next_fork = (*self).id % (*self).n_fork;
	(*self).last_meal = get_timestamp_ms();
	if (!locked_printf("%ld %d is eating\n", self))
	{
		pthread_mutex_unlock(&forks[current_fork].mut);
		pthread_mutex_unlock(&forks[next_fork].mut);
		return (-1);
	}
	precision_sleep(checkpoint, &(*self).time_to_eat);
	pthread_mutex_unlock(&forks[current_fork].mut);
	pthread_mutex_unlock(&forks[next_fork].mut);
	return (get_timestamp_ms());
}

int	start_sleeping(t_thread_info *self, unsigned long *checkpoint)
{
	unsigned int	time_to_sleep;

	time_to_sleep = (*self).time_to_sleep;
	locked_printf("%ld %d is sleeping\n", self);
	if ((get_timestamp_ms() + time_to_sleep) - (*self).last_meal >= (*self).time_to_die)
	{
		time_to_sleep = (*self).time_to_die - (get_timestamp_ms() - (*self).last_meal);
		precision_sleep(checkpoint, &time_to_sleep);
		return (-1);
	}
	precision_sleep(checkpoint, &time_to_sleep);
	return (1);
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
		precision_sleep(&checkpoint, &(*self).time_to_die);
		pthread_mutex_unlock(&forks[(*self).id - 1].mut);
		philo_dead(self);
		return (NULL);
	}
	checkpoint = get_timestamp_ms();
	(*self).last_meal = get_timestamp_ms();
	while (i != (*self).n_time_eat && (get_timestamp_ms() - (*self).last_meal) <= (*self).time_to_die)
	{	
		locked_printf("%ld %d is thinking\n", self);
		checkpoint = get_timestamp_ms();
		if ((get_timestamp_ms() - (*self).last_meal) > (*self).time_to_die)
			break ;
		if (take_forks(self) == -1)
			break ;	
		if (start_eating(self, &checkpoint) == -1)
			break ; 	
		i = i + 1;
		if (start_sleeping(self, &checkpoint) == -1)
			break ;
	}
	if (i != (*self).n_time_eat)
		philo_dead(self);
	return (NULL);
}

void	run(t_thread_info *philo, int n_philo)
{
	int		i;
	unsigned long	start;
	pthread_mutex_t	locked;
	int		deadbeef;
	int		started;

	i = -1;
	deadbeef = 0;
	started = 0;
	pthread_mutex_init(&locked, NULL);	
	start = get_timestamp_ms();
	while ((++i) < n_philo)
	{
		philo[i].started_at = &start;
		philo[i].locked = &locked;
		philo[i].deadbeef = &deadbeef;
		philo[i].started = &started;
		pthread_create(&philo[i].thread, NULL, &start_routine, &philo[i]);
	}
	i = -1;
	while ((++i) < n_philo)
		pthread_join(philo[i].thread, NULL);
	pthread_mutex_destroy(&locked);
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
