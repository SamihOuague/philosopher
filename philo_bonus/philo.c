/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 21:17:23 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/27 21:18:16 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	philo_routine(t_philo *self)
{
	send_msg(self, 0);
	if ((*self).n_fork <= 1)
	{
		sem_wait((*self).forks);
		send_msg(self, 1);
		usleep((*self).time_to_die * 1000);
		sem_post((*self).forks);
		return (1);
	}	
	sem_wait((*self).fork_lock);
	if ((*(*self).die_lock).__align == 0)
		return (1);
	if (wait_forks(self))
		return (1);
	sem_post((*self).fork_lock);
	if ((get_timestamp_ms() - (*self).last_meal) > (*self).time_to_die)
		return (1);
	(*self).last_meal = get_timestamp_ms();
	send_msg(self, 2);
	if (precision_sleep((*self).time_to_eat, self))
		return (1);
	if ((get_timestamp_ms() - (*self).last_meal) > (*self).time_to_die)
		return (1);
	go_to_sleep(self);
	return (0);
}

int	philo_proc(t_philo *philo, int index)
{
	t_philo	*self;
	pid_t	pid;	
	int		i;

	self = &philo[index];
	i = 0;
	pid = fork();
	if ((*self).id % 2 == 0)
		usleep(1000);
	if (pid == 0)
	{
		i = -1;
		(*self).last_meal = get_timestamp_ms();
		while ((++i) != (*self).n_time_eat)
		{	
			if ((get_timestamp_ms() - (*self).last_meal) > (*self).time_to_die)
				break ;
			if (philo_routine(self))
				break ;
		}
		clean_mem_prog(self, philo, i);
	}
	return (pid);
}

void	init_sem(t_shared *shared, int n_fork)
{
	sem_unlink("forks1");
	sem_unlink("fork_lock");
	sem_unlink("die_lock");
	sem_unlink("msg_lock");
	(*shared).forks = sem_open("forks1", O_CREAT, 0666, n_fork);
	(*shared).fork_sem = sem_open("fork_lock", O_CREAT, 0666, 1);
	(*shared).die_sem = sem_open("die_lock", O_CREAT, 0666, 1);
	(*shared).msg_sem = sem_open("msg_lock", O_CREAT, 0666, 1);
}

void	init_philo_sem(t_philo *p, t_shared shrd, unsigned long s_at, int n)
{
	int	i;

	i = -1;
	while ((++i) < n)
	{
		p[i].id = i + 1;
		p[i].forks = shrd.forks;
		p[i].msg_lock = shrd.msg_sem;
		p[i].started_at = s_at;
		p[i].fork_lock = shrd.fork_sem;
		p[i].die_lock = shrd.die_sem;
		if (philo_proc(p, i) == 0)
			break ;
	}
}

int	wait_for_philos(t_shared shared, t_philo *philo, int *args)
{
	unsigned long	t;	
	int				i;
	int				exit_status;
	int				deadbeef;

	i = -1;
	deadbeef = 0;
	exit_status = 0;
	while ((++i) < (*philo).n_fork)
	{
		waitpid(-1, &exit_status, 0);
		if (WEXITSTATUS(exit_status) != 0 && !deadbeef)
		{
			sem_wait(shared.msg_sem);
			t = get_timestamp_ms() - (*philo).started_at;
			printf("%ld %d died\n", t, WEXITSTATUS(exit_status));
			deadbeef = 1;
			sem_post(shared.msg_sem);
			sem_wait(shared.die_sem);
		}
	}
	clear_parent_sem(shared);
	free(philo);
	free(args);
	return (exit_status);
}
