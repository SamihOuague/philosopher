/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 04:25:29 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/27 22:26:27 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	take_forks(t_philo *self)
{
	int	current_fork;
	int	next_fork;

	current_fork = (*self).id - 1;
	next_fork = (*self).id % (*self).n_fork;
	if (next_fork < current_fork)
	{
		current_fork = next_fork;
		next_fork = (*self).id - 1;
	}
	if ((*self).n_fork <= 1)
	{
		pthread_mutex_lock(&(*self).forks[current_fork].mut);
		send_msg(self, 0);
		usleep((*self).time_to_die * 1000);
		pthread_mutex_unlock(&(*self).forks[current_fork].mut);
		return ;
	}
	pthread_mutex_lock(&(*self).forks[next_fork].p_mut);
	pthread_mutex_lock(&(*self).forks[current_fork].mut);
	send_msg(self, 0);
	pthread_mutex_lock(&(*self).forks[next_fork].mut);
	send_msg(self, 0);
	pthread_mutex_unlock(&(*self).forks[next_fork].p_mut);
}

void	get_meal(t_philo *self)
{
	send_msg(self, 2);
	precision_sleep((*self).time_to_eat);
}

void	get_sleep(t_philo *self)
{
	int	current_fork;
	int	next_fork;

	current_fork = (*self).id - 1;
	next_fork = (*self).id % (*self).n_fork;
	if (next_fork < current_fork)
	{
		current_fork = next_fork;
		next_fork = (*self).id - 1;
	}
	send_msg(self, 3);
	pthread_mutex_unlock(&(*self).forks[current_fork].mut);
	pthread_mutex_unlock(&(*self).forks[next_fork].mut);
	precision_sleep((*self).time_to_sleep);
}

void	*philo_routine(void *arg)
{	
	t_philo			*self;

	self = arg;
	if ((*self).id % 2 == 0)
		usleep(15000);
	while (1)
	{
		send_msg(self, 1);
		pthread_mutex_lock((*self).locked);
		if (*(*self).deadbeef == 1)
		{
			pthread_mutex_unlock((*self).locked);
			break ;
		}
		pthread_mutex_unlock((*self).locked);
		take_forks(self);
		if ((*self).n_fork <= 1)
			break ;
		get_meal(self);
		get_sleep(self);
	}
	return (NULL);
}

void	create_philo_thread(t_philo *philo, t_shared *shared)
{
	int	i;

	i = -1;
	(*shared).deadbeef = 0;
	(*shared).msg_queue = NULL;
	pthread_mutex_init(&(*shared).locked, NULL);
	pthread_mutex_init(&(*shared).msg_lock, NULL);
	pthread_mutex_lock(&(*shared).msg_lock);
	(*shared).started_at = get_timestamp_ms();
	while ((++i) < (*philo).n_fork)
	{
		philo[i].started_at = (*shared).started_at;
		philo[i].msg_lock = &(*shared).msg_lock;
		philo[i].locked = &(*shared).locked;
		philo[i].msg_queue = &(*shared).msg_queue;
		philo[i].deadbeef = &(*shared).deadbeef;
		pthread_mutex_init(&philo[i].meal_lock, NULL);
		pthread_create(&philo[i].thread, NULL, &philo_routine, &philo[i]);
	}
}
