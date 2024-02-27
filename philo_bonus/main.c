/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 03:44:09 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/27 07:00:26 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	philo_proc(t_philo *philo, int index)
{
	t_philo	*self;
	pid_t	pid;	
	char	n[6];
	int		i;
	int		current_fork;
	int		next_fork;
	int		exit_code;

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
			send_msg(self, 0);
			if ((*self).n_fork <= 1)
			{
				sem_wait((*self).forks);
				send_msg(self, 1);
				usleep((*self).time_to_die * 1000);
				sem_post((*self).forks);
				break ;
			}	
			sem_wait((*self).fork_lock);
			if ((*(*self).die_lock).__align == 0)
				break ;
			if (wait_forks(self))
				break ;
			sem_post((*self).fork_lock);
			if ((get_timestamp_ms() - (*self).last_meal) > (*self).time_to_die)
				break ;
			(*self).last_meal = get_timestamp_ms();
			send_msg(self, 2);
			if (precision_sleep((*self).time_to_eat, self))
				break ;
			if ((get_timestamp_ms() - (*self).last_meal) > (*self).time_to_die)
				break ;
			send_msg(self, 3);
			if ((get_timestamp_ms() - (*self).last_meal) > (*self).time_to_die)
				break ;
			sem_post((*self).forks);
			sem_post((*self).forks);
			if (precision_sleep((*self).time_to_sleep, self))
				break ;
		}
		sem_post((*self).fork_lock);
		sem_post((*self).forks);
		sem_post((*self).forks);
		exit_code = 0;
		if (i != (*self).n_time_eat)
			exit_code = (*self).id;
		sem_close((*self).forks);
		sem_unlink("forks1");
		free((*self).args);
		sem_close((*self).msg_lock);
		sem_unlink("msg_lock");
		sem_close((*self).fork_lock);
		sem_unlink("fork_lock");
		sem_close((*self).die_lock);
		sem_unlink("die_lock");
		free(philo);
		exit(exit_code);
	}
	return (pid);
}

int	main(int argc, char **argv)
{	
	unsigned long	t;
	unsigned long	started_at;
	t_monitor		monitor;
	t_philo			*philo;
	pid_t			pid;
	sem_t			*forks;
	sem_t			*msg_sem;
	sem_t			*fork_sem;
	sem_t			*die_sem;
	char			*name;
	char			n[6];
	int				i;
	int				*args;
	int				deadbeef;
	int				exit_status;

	if (!(argc == 5 || argc == 6))
		return (1);
	i = -1;
	deadbeef = 0;
	memset(n, '\0', 6);
	ft_strcpy(n, "fork0");
	started_at = get_timestamp_ms();
	args = check_args(argc, argv);
	philo = init_philo(args);
	if (philo == NULL)
		return (1);
	sem_unlink("forks1");
	sem_unlink("fork_lock");
	sem_unlink("die_lock");
	forks = sem_open("forks1", O_CREAT, 0666, args[0]);
	fork_sem = sem_open("fork_lock", O_CREAT, 0666, 1);
	die_sem = sem_open("die_lock", O_CREAT, 0666, 1);
	i = -1;
	if (philo == NULL)
		return (1);
	sem_unlink("msg_lock");
	msg_sem = sem_open("msg_lock", O_CREAT, 0666, 1);
	while ((++i) < args[0])
	{
		philo[i].id = i + 1;
		philo[i].forks = forks;
		philo[i].msg_lock = msg_sem;
		philo[i].started_at = started_at;
		philo[i].fork_lock = fork_sem;
		philo[i].die_lock = die_sem;
		if (philo_proc(philo, i) == 0)
			break ;
	}
	i = -1;
	while ((++i) < args[0])
	{
		pid = waitpid(-1, &exit_status, 0);
		if (WEXITSTATUS(exit_status) != 0 && !deadbeef)
		{
			sem_wait(msg_sem);
			t = get_timestamp_ms() - started_at;
			printf("%ld %d died\n", t, WEXITSTATUS(exit_status));
			deadbeef = 1;
			sem_post(msg_sem);
			sem_wait(die_sem);
		}
	}
	i = -1;
	sem_close(forks);
	sem_unlink("forks1");
	sem_close(msg_sem);
	sem_unlink("msg_lock");
	sem_close(fork_sem);
	sem_unlink("fork_lock");
	sem_close(die_sem);
	sem_unlink("die_lock");
	free(args);
	free(philo);
	return (0);
}
