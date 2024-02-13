/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 00:28:47 by  souaguen         #+#    #+#             */
/*   Updated: 2024/02/13 03:25:37 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*forks_init(unsigned int n_philo)
{
	int	i;
	t_fork	*forks;

	i = -1;
	forks = malloc(sizeof(t_fork) * n_philo);
	if (forks == NULL)
		return (NULL);
	while ((++i) < n_philo)
		pthread_mutex_init(&forks[i].mut, NULL);
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

void	free_forks(t_fork *forks, int n_forks)
{
	int	i;
	
	i = -1;
	while ((++i) < n_forks)
		pthread_mutex_destroy(&forks[i].mut);
	free(forks);
}
