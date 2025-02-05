/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 06:47:11 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/29 06:01:45 by souaguen         ###   ########.fr       */
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
	if ((*(*self).die_lock).__align != 0)
		printf("%ld %d %s\n", t - (*self).started_at, (*self).id, str);
	sem_post((*self).msg_lock);
}

int	wait_forks(t_philo *self)
{
	sem_t	*forks;

	forks = (*self).forks;
	while ((*forks).__align < 2)
	{
		if ((int)(get_timestamp_ms() - (*self).last_meal) > (*self).time_to_die)
			return (1);
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

void	clean_mem_prog(t_philo *self, t_philo *philo, int n_eat)
{
	int		exit_code;

	sem_post((*self).fork_lock);
	sem_post((*self).forks);
	sem_post((*self).forks);
	sem_close((*self).forks);
	sem_unlink("forks1");
	free((*self).args);
	sem_close((*self).msg_lock);
	sem_unlink("msg_lock");
	sem_close((*self).fork_lock);
	sem_unlink("fork_lock");
	sem_close((*self).die_lock);
	sem_unlink("die_lock");
	exit_code = 0;
	if (n_eat != (*self).n_time_eat)
		exit_code = (*self).id;
	free(philo);
	exit(exit_code);
}

int	go_to_sleep(t_philo *self)
{
	send_msg(self, 3);
	if ((int)(get_timestamp_ms() - (*self).last_meal) > (*self).time_to_die)
		return (1);
	sem_post((*self).forks);
	sem_post((*self).forks);
	if (precision_sleep((*self).time_to_sleep, self))
		return (1);
	return (0);
}
