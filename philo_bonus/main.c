/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 03:44:09 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/22 20:51:19 by souaguen         ###   ########.fr       */
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

typedef struct s_philo
{
	int	id;
	int	n_forks;
	pid_t	pid;
	sem_t	**forks;
	sem_t	*locked;
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

void	philo_proc(sem_t *s[2], sem_t *s_p, unsigned long checkpoint, unsigned int id)
{
	pid_t	pid;
	int		i;

	i = 0;
	pid = fork();
	usleep(10000);
	if (pid == 0)
	{
		i = -1;
		while ((++i) < 5)
		{
			sem_wait(s[id - 1]);
			//sem_wait(s_p);
			//printf("%d has taken a fork\n", id);
			//sem_post(s_p);
			sem_wait(s[id % 2]);
			
			//sem_wait(s_p);
			//printf("%d has taken a fork\n", id);
			//sem_post(s_p);
	
			
			sem_wait(s_p);
			printf("%d is eating\n", id);
			sem_post(s_p);
			usleep(200000);
			
			sem_post(s[id - 1]);
			sem_post(s[id % 2]);
			
			sem_wait(s_p);
			printf("%d is sleeping\n", id);
			sem_post(s_p);
			
			usleep(200000);
		}
		exit(0);
	}
	else
		waitpid(-1, NULL, 0);
}

int	main(int argc, char **argv)
{	
	unsigned long	t;
	int	i;
	char	*name1;
	char	*name2;
	char	*p_name;
	sem_t	*s[2];
	sem_t	*s_p;

	i = 0;
	name1 = "fork1";
	name2 = "fork2";
	p_name = "print";
	sem_unlink(name1);	
	sem_unlink(name2);
	s[0] = sem_open(name1, O_CREAT, 0666, 1);	
	s[1] = sem_open(name2, O_CREAT, 0666, 1);
	s_p = sem_open(p_name, O_CREAT, 0666, 1);
	t = get_timestamp_ms();
	while ((++i) <= 2)
		philo_proc(s, s_p, t, i);
	waitpid(-1, NULL, 0);
	sem_close(s[0]);
	sem_close(s[1]);
	sem_unlink(name1);
	sem_unlink(name2);
	sem_unlink(p_name);
	return (0);
}
