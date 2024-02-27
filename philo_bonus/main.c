/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 03:44:09 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/27 21:19:37 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	clear_parent_sem(t_shared shared)
{
	sem_close(shared.forks);
	sem_unlink("forks1");
	sem_close(shared.msg_sem);
	sem_unlink("msg_lock");
	sem_close(shared.fork_sem);
	sem_unlink("fork_lock");
	sem_close(shared.die_sem);
	sem_unlink("die_lock");
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
		return (1);
	init_sem(&shared, args[0]);
	init_philo_sem(philo, shared, get_timestamp_ms(), args[0]);
	wait_for_philos(shared, philo, args);
	return (0);
}
