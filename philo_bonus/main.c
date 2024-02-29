/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 03:44:09 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/29 05:59:53 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	clear_parent_sem(t_shared shared)
{
	if (shared.forks != NULL)
		sem_close(shared.forks);
	sem_unlink("forks1");
	if (shared.msg_sem != NULL)
		sem_close(shared.msg_sem);
	sem_unlink("msg_lock");
	if (shared.fork_sem != NULL)
		sem_close(shared.fork_sem);
	sem_unlink("fork_lock");
	if (shared.die_sem != NULL)
		sem_close(shared.die_sem);
	sem_unlink("die_lock");
}

int	check_sem(t_shared *shared)
{
	if ((*shared).forks == NULL
		|| (*shared).msg_sem == NULL
		|| (*shared).fork_sem == NULL
		|| (*shared).die_sem == NULL)
		return (1);
	return (0);
}

int	main(int argc, char **argv)
{	
	t_philo			*philo;
	t_shared		shared;
	int				*args;

	if (!(argc == 5 || argc == 6))
		return (1);
	args = check_args(argc, argv);
	philo = init_philo(args);
	if (philo == NULL)
	{
		if (args != NULL)
			free(args);
		return (1);
	}
	init_sem(&shared, args[0]);
	if (check_sem(&shared)
		|| !init_philo_sem(philo, shared, get_timestamp_ms(), args[0]))
	{
		clear_parent_sem(shared);
		free(args);
		free(philo);
		return (1);
	}
	wait_for_philos(shared, philo, args);
	return (0);
}
