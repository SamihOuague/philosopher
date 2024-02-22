/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 22:13:26 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/22 03:37:54 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*monitor_routine(void *arg)
{
	unsigned long	*last_meal;
	unsigned long	t;	
	t_monitor		*self;	
	t_msg			*msg;	
	t_list			*tmp;	
	char			*str;
	int				*n_time_eat;
	int				i;
	int				c;
	int				started_at;

	i = -1;
	self = arg;
	msg = NULL;
	t = get_timestamp_ms();	
	started_at = (*self).started_at;
	n_time_eat = malloc(sizeof(int) * (*self).n_philo);
	last_meal = malloc(sizeof(unsigned long) * (*self).n_philo);
	while ((++i) < (*self).n_philo)
	{
		n_time_eat[i] = 0;
		last_meal[i] = t;
	}
	while (1)
	{
		usleep(100);
		pthread_mutex_lock((*self).msg_lock);
		while (*(*self).msg_queue != NULL)
		{
			tmp = ft_pop((*self).msg_queue);
			msg = (*tmp).content;
			if ((*self).philos[(*msg).id - 1].n_time_eat
				!= n_time_eat[(*msg).id - 1])
			{
				if ((*msg).status == 0)
					str = "has taken a fork";
				else if ((*msg).status == 1)
					str = "is thinking";
				else if ((*msg).status == 2)
				{	
					last_meal[(*msg).id - 1] = (*msg).timestamp;
					str = "is eating";
				}
				else if ((*msg).status == 3)
				{
					n_time_eat[(*msg).id - 1] += 1;
					str = "is sleeping";
				}
				t = (*msg).timestamp - (*self).philos[0].started_at;
				printf("%ld %d %s\n", t, (*msg).id, str);
			}
			free(msg);
			free(tmp);
		}
		pthread_mutex_unlock((*self).msg_lock);
		t = get_timestamp_ms();
		i = -1;
		c = 0;
		while ((++i) < (*self).n_philo)
		{
			if ((t - last_meal[i]) > (*self).philos[i].time_to_die
				&& (*self).philos[i].n_time_eat != n_time_eat[i])
			{
				pthread_mutex_lock((*self).msg_lock);
				printf("%ld %d died\n", t - (*self).philos[i].started_at, i + 1);
				pthread_mutex_unlock((*self).msg_lock);
				free(last_meal);
				free(n_time_eat);
				pthread_mutex_lock((*self).locked);
				*(*self).deadbeef = 1;
				pthread_mutex_unlock((*self).locked);
				return (NULL);
			}
			else if ((*self).philos[i].n_time_eat == n_time_eat[i])
				c++;
		}
		usleep(1000);
		if (c == (*self).n_philo)
		{
			pthread_mutex_lock((*self).locked);
			*(*self).deadbeef = 1;
			pthread_mutex_unlock((*self).locked);
			free(last_meal);
			free(n_time_eat);
			return (NULL);
		}
	}
	return (NULL);
}

void	*philo_routine(void *arg)
{	
	unsigned long	checkpoint;
	t_philo			*self;
	int				current_fork;
	int				next_fork;
	int				think;

	think = 1;
	self = arg;
	current_fork = (*self).id - 1;
	next_fork = (*self).id % (*self).n_fork;
	if (next_fork < current_fork)
	{
		current_fork = next_fork;
		next_fork = (*self).id - 1;
	}
	while (1)
	{		
		if (think)
			send_msg(self, 1);
		pthread_mutex_lock((*self).locked);
		if (*(*self).deadbeef == 1)
		{
			pthread_mutex_unlock((*self).locked);
			break ;
		}
		pthread_mutex_unlock((*self).locked);
		pthread_mutex_lock(&(*self).forks[current_fork].mut);
		if ((*self).n_fork <= 1)
		{	
			send_msg(self, 0);
			pthread_mutex_unlock(&(*self).forks[current_fork].mut);
			return (NULL);
		}
		send_msg(self, 0);
		pthread_mutex_lock(&(*self).forks[next_fork].mut);
		send_msg(self, 0);
		checkpoint = get_timestamp_ms();
		send_msg(self, 2);
		precision_sleep((*self).time_to_eat);
		checkpoint = get_timestamp_ms();
		send_msg(self, 3);
		pthread_mutex_unlock(&(*self).forks[current_fork].mut);
		pthread_mutex_unlock(&(*self).forks[next_fork].mut);
		checkpoint = (get_timestamp_ms() - checkpoint);
		precision_sleep((*self).time_to_sleep - checkpoint);
		think = 1;
	}
	return (NULL);
}

void	run(t_philo *philo)
{	
	unsigned long	started_at;
	pthread_mutex_t	locked;
	pthread_mutex_t	msg_lock;
	t_monitor		monitor;
	t_list			*msg_queue;
	t_list			*tmp;
	int				deadbeef;
	int				i;

	if (philo == NULL)
		return ;
	i = -1;
	msg_queue = NULL;
	pthread_mutex_init(&locked, NULL);
	pthread_mutex_init(&msg_lock, NULL);
	started_at = get_timestamp_ms();
	monitor.msg_queue = &msg_queue;
	monitor.msg_lock = &msg_lock;
	monitor.n_philo = (*philo).n_fork;
	monitor.philos = philo;
	monitor.deadbeef = &deadbeef;
	monitor.locked = &locked;
	monitor.started_at = started_at;
	deadbeef = 0;
	while ((++i) < (*philo).n_fork)
	{
		philo[i].started_at = started_at;
		philo[i].msg_lock = &msg_lock;
		philo[i].locked = &locked;
		philo[i].msg_queue = &msg_queue;
		philo[i].deadbeef = &deadbeef;
		pthread_mutex_init(&philo[i].meal_lock, NULL);
		pthread_create(&philo[i].thread, NULL, &philo_routine, &philo[i]);
	}	
	pthread_create(&monitor.thread, NULL, &monitor_routine, &monitor);
	i = -1;
	while ((++i) < (*philo).n_fork)
		pthread_join(philo[i].thread, NULL);
	pthread_join(monitor.thread, NULL);
	i = -1;
	pthread_mutex_destroy(&locked);
	pthread_mutex_destroy(&msg_lock);
	while (msg_queue != NULL)
	{
		tmp = ft_pop(&msg_queue);
		free((*tmp).content);
		free(tmp);
	}
	i = -1;
	while ((++i) < (*philo).n_fork)
		pthread_mutex_destroy(&philo[i].meal_lock);
	free_forks((*philo).forks, (*philo).n_fork);
	free(philo);
}

int	main(int argc, char **argv)
{
	int	*args;

	args = NULL;
	if (argc < 5 || argc > 6)
	{
		ft_putstr_fd("Number of arguments must be 5 or 6\n", 2);
		return (1);
	}
	args = check_args(argc, argv);
	if (args == NULL)
	{
		ft_putstr_fd("Invalid argument !\n", 2);
		return (1);
	}
	run(init_philo(args));
	free(args);
	return (0);
}
