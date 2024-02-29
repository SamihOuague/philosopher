/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 22:13:26 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/29 01:44:30 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	empty_queue(t_list **queue)
{
	t_list	*tmp;

	tmp = NULL;
	if (queue == NULL || *queue == NULL)
		return ;
	while (*queue != NULL)
	{
		tmp = ft_pop(queue);
		free((*tmp).content);
		free(tmp);
	}
}

void	run(t_philo *philo, int *args)
{
	t_monitor		monitor;
	t_shared		shared;
	int				i;
	int				er;

	if (philo == NULL)
		return ;
	i = -1;
	create_philo_thread(philo, &shared, args);
	er = create_mon_thread(&monitor, philo, &shared);
	while ((++i) < (*philo).n_fork)
		pthread_join(philo[i].thread, NULL);
	i = -1;
	if (!er)
		pthread_join(monitor.thread, NULL);
	pthread_mutex_destroy(&shared.locked);
	pthread_mutex_destroy(&shared.msg_lock);
	empty_queue(&shared.msg_queue);
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
	run(init_philo(args), args);
	free(args);
	return (0);
}
