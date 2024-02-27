/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 00:28:47 by  souaguen         #+#    #+#             */
/*   Updated: 2024/02/27 22:14:19 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	*check_args(int argc, char **argv)
{
	int	i;
	int	*args;

	i = 0;
	args = malloc(sizeof(int) * 5);
	if (args == NULL)
		return (NULL);
	args[4] = -1;
	while ((++i) < argc)
	{
		args[i - 1] = ft_atoi(argv[i]);
		if (!is_numeric(argv[i])
			|| (args[i - 1] >= 1000000 || args[i - 1] <= 0))
		{
			free(args);
			args = NULL;
			return (NULL);
		}
	}
	return (args);
}

void	free_forks(t_fork *forks, int n_forks)
{
	int	i;

	i = -1;
	while ((++i) < n_forks)
	{	
		pthread_mutex_destroy(&forks[i].p_mut);
		pthread_mutex_destroy(&forks[i].mut);
	}
	free(forks);
}

//error management to do
t_fork	*init_forks(unsigned int n_philo)
{
	t_fork	*forks;	
	int		i;

	i = -1;
	forks = malloc(sizeof(t_fork) * n_philo);
	if (forks == NULL)
		return (NULL);
	while ((++i) < n_philo)
	{
		forks[i].free_fork = 0;
		pthread_mutex_init(&forks[i].p_mut, NULL);
		pthread_mutex_init(&forks[i].mut, NULL);
	}
	return (forks);
}

t_philo	*init_philo(int *args)
{
	t_philo	*philo;
	t_fork	*forks;
	int		i;

	philo = malloc(sizeof(t_philo) * args[0]);
	if (philo == NULL)
		return (NULL);
	forks = init_forks(args[0]);
	if (forks == NULL)
	{
		free(philo);
		return (NULL);
	}
	i = -1;
	while ((++i) < args[0])
	{
		philo[i].id = i + 1;
		philo[i].time_to_die = args[1];
		philo[i].time_to_eat = args[2];
		philo[i].time_to_sleep = args[3];
		philo[i].n_time_eat = args[4];
		philo[i].n_fork = args[0];
		philo[i].forks = forks;
	}
	return (philo);
}

void	send_msg(t_philo *self, int status)
{
	t_msg	*msg;

	msg = malloc(sizeof(t_msg));
	if (msg == NULL)
		return ;
	(*msg).id = (*self).id;
	(*msg).status = status;
	pthread_mutex_lock((*self).msg_lock);
	(*msg).timestamp = get_timestamp_ms();
	ft_lstadd_back((*self).msg_queue, ft_lstnew(msg));
	pthread_mutex_unlock((*self).msg_lock);
}
