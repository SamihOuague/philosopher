/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 06:47:11 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/27 06:50:21 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	send_msg(t_philo *self, int status)
{
	unsigned long	t;
	char			*str;

	str = "";
	if ((*(*self).die_lock).__align == 0)
		return ;
	sem_wait((*self).msg_lock);
	t = get_timestamp_ms();
	if (status == 1)
		str = "has taken a fork";
	else if (status == 0)
		str = "is thinking";
	else if (status == 2)
		str = "is eating";
	else if (status == 3)
		str = "is sleeping";
	printf("%ld %d %s\n", t - (*self).started_at, (*self).id, str);
	sem_post((*self).msg_lock);
}

int	wait_forks(t_philo *self)
{
	sem_t	*forks;

	forks = (*self).forks;
	if ((*forks).__align > (*self).n_fork)
		return (1);
	while ((*forks).__align < 2)
	{
		if ((get_timestamp_ms() - (*self).last_meal) > (*self).time_to_die)
			break ;
		usleep(1000);
	}
	sem_wait((*self).forks);
	send_msg(self, 1);
	sem_wait((*self).forks);
	send_msg(self, 1);
	return (0);
}

t_philo	*init_philo(int *args)
{
	t_philo	*philo;
	int		i;

	if (args == NULL)
		return (NULL);
	philo = malloc(sizeof(t_philo) * args[0]);
	if (philo == NULL)
		return (NULL);
	i = -1;
	while ((++i) < args[0])
	{
		philo[i].id = i + 1;
		philo[i].time_to_die = args[1];
		philo[i].time_to_eat = args[2];
		philo[i].time_to_sleep = args[3];
		philo[i].n_time_eat = args[4];
		philo[i].n_fork = args[0];
		philo[i].args = args;
	}
	return (philo);
}
