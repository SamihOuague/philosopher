/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 03:44:09 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/23 22:00:13 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_philo
{
	unsigned int	last_meal;
	int	id;
	int	n_fork;
	pid_t	pid;
	sem_t	***forks;
	sem_t	*msg_lock;
	int	time_to_die;
	int	time_to_eat;
	int	time_to_sleep;
	int	n_time_eat;
}	t_philo;

unsigned long	get_timestamp_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (((tv.tv_sec * 1000000) + tv.tv_usec) / 1000);
}

void	philo_proc(t_philo *self)
{
	pid_t	pid;
	int		i;
	int		current_fork;
	int		next_fork;

	i = 0;
	pid = fork();
	current_fork = (*self).id - 1;
	next_fork = (*self).id % (*self).n_fork;
	if ((*self).id % 2 == 0)
	{
	//	current_fork = next_fork;
	//	next_fork = (*self).id - 1;
		usleep(10000);
	}
	if (pid == 0)
	{

		i = -1;
		while ((++i) < (*self).n_time_eat)
		{
			//sem_wait(*(*self).forks[current_fork]);		
			sem_wait(*(*self).forks[next_fork]);	

			sem_wait((*self).msg_lock);
			printf("%d has taken a fork\n", (*self).id);
			sem_post((*self).msg_lock);
			

			sem_wait((*self).msg_lock);
			printf("%d has taken a fork\n", (*self).id);
			sem_post((*self).msg_lock);

			sem_wait((*self).msg_lock);
			printf("%d is eating\n", (*self).id);
			sem_post((*self).msg_lock);
			usleep(200000);

			sem_wait((*self).msg_lock);
			printf("%d is sleeping\n", (*self).id);
			sem_post((*self).msg_lock);

			//sem_post(*(*self).forks[current_fork]);	
			sem_post(*(*self).forks[next_fork]);
			usleep(200000);
		}
		exit(0);
	}
}

int	ft_strlen(char *str)
{
	int	i;
	
	i = 0;
	while (*(str + i) != '\0')
		i++;
	return (i);
}

int	ft_strcpy(char *dest, char *src)
{
	int	i;

	i = -1;
	while (src[(++i)] != '\0')
		dest[i] = src[i];
	dest[i] = '\0';
	return (i);
}

int	main(int argc, char **argv)
{	
	unsigned long	t;
	int	i;
	char	n[6];
	char	*name;
	sem_t	**forks;
	sem_t	*msg_sem;
	t_philo	philo[4];

	i = -1;
	ft_strcpy(n, "fork0");
	forks = malloc(sizeof(sem_t *) * 4);
	memset(n, '\0', 6);
	while ((++i) < 4)
	{
		name = malloc(sizeof(char) * 6);
		n[4] = '1' + i;
		ft_strcpy(name, n);
		sem_unlink(name);
		forks[i] = sem_open(name, O_CREAT, 0666, 1);
		free(name);
	}
	msg_sem = sem_open("msg_lock", O_CREAT, 0666, 1);	
	i = -1;
	while ((++i) < 4)
	{
		philo[i].id = i + 1; 
		philo[i].n_fork = 4;
		philo[i].forks = &forks;
		philo[i].msg_lock = msg_sem;
		philo[i].time_to_die = 1600;
		philo[i].time_to_eat = 400;
		philo[i].time_to_sleep = 400;
		philo[i].n_time_eat = 5;	
		philo_proc(&philo[i]);
	}
	i = -1;
	while ((++i) < 4)
		waitpid(-1, NULL, 0);
	i = -1;
	while ((++i) < 4)
	{
		n[4] = '1' + i;
		sem_close(forks[i]);
		sem_unlink(n);
	}

	sem_close(msg_sem);
	sem_unlink("msg_lock");
	return (0);
}
