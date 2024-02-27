/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 05:24:13 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/27 05:51:30 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

char	*get_msg(t_msg *msg, unsigned long *last_meal, int *n_time_eat)
{
	char	*str;

	str = "";
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
	return (str);
}

void	print_msgs(t_monitor *self, unsigned long *last_meal, int *n_time_eat)
{	
	unsigned long	t;	
	t_msg			*msg;	
	t_list			*tmp;
	char			*str;

	pthread_mutex_lock((*self).msg_lock);
	while (*(*self).msg_queue != NULL)
	{
		tmp = ft_pop((*self).msg_queue);
		msg = (*tmp).content;
		if ((*self).philos[(*msg).id - 1].n_time_eat
			!= n_time_eat[(*msg).id - 1])
		{
			t = (*msg).timestamp - (*self).philos[0].started_at;
			str = get_msg(msg, last_meal, n_time_eat);
			printf("%ld %d %s\n", t, (*msg).id, str);
		}
		free(msg);
		free(tmp);
	}
	pthread_mutex_unlock((*self).msg_lock);
}

int	check_death(t_monitor *self, unsigned long *last_meal, int *n_time_eat)
{
	unsigned long	t;
	int				c;
	int				i;

	c = 0;
	i = -1;
	t = get_timestamp_ms();
	while ((++i) < (*self).n_philo)
	{
		if ((t - last_meal[i]) > (*self).philos[i].time_to_die
			&& (*self).philos[i].n_time_eat != n_time_eat[i])
		{
			pthread_mutex_lock((*self).msg_lock);
			printf("%ld %d died\n", t - (*self).philos[i].started_at, i + 1);
			pthread_mutex_unlock((*self).msg_lock);
			return (-1);
		}
		else if ((*self).philos[i].n_time_eat == n_time_eat[i])
			c++;
	}
	return (c);
}

void	*monitor_routine(void *arg)
{
	unsigned long	*last_meal;
	t_monitor		*self;	
	int				*n_time_eat;
	int				i;

	i = -1;
	self = arg;
	init_track(&last_meal, &n_time_eat, (*self).n_philo);
	while (1)
	{
		usleep(100);
		print_msgs(self, last_meal, n_time_eat);
		i = check_death(self, last_meal, n_time_eat);
		if (i == -1 || i == (*self).n_philo)
			break ;
		usleep(1000);
	}
	pthread_mutex_lock((*self).locked);
	*(*self).deadbeef = 1;
	pthread_mutex_unlock((*self).locked);
	free(last_meal);
	free(n_time_eat);
	return (NULL);
}

void	create_mon_thread(t_monitor *monitor, t_philo *philo, t_shared *shared)
{
	(*monitor).msg_queue = &(*shared).msg_queue;
	(*monitor).msg_lock = &(*shared).msg_lock;
	(*monitor).n_philo = (*philo).n_fork;
	(*monitor).philos = philo;
	(*monitor).deadbeef = &(*shared).deadbeef;
	(*monitor).locked = &(*shared).locked;
	(*monitor).started_at = (*shared).started_at;
	pthread_create(&(*monitor).thread, NULL, &monitor_routine, monitor);
	pthread_mutex_unlock(&(*shared).msg_lock);
}
