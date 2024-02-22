/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/22 03:44:09 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/22 05:06:14 by souaguen         ###   ########.fr       */
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

unsigned long	get_timestamp_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (((tv.tv_sec * 1000000) + tv.tv_usec) / 1000);
}

void	philo(int id, unsigned long checkpoint, sem_t *s)
{
	pid_t	pid;
	int		i;

	pid = fork();
	if (pid == 0)
	{
		printf("%ld %d Child Process starts\n", get_timestamp_ms() - checkpoint, id);
		i = -1;
		while ((++i) < 1000)
			usleep(1000);	
		printf("%ld %d Child Process ends\n", get_timestamp_ms() - checkpoint, id);
		printf("%p\n", s);
		exit(0);
	}
}

int	main(int argc, char **argv)
{
	int	i;
	sem_t	*s;

	s = NULL;
	s = sem_open("fork2", O_CREAT, 0666, 0);
	i = 0;
	while ((++i) < 6)
		philo(i, get_timestamp_ms(), s);
	waitpid(-1, NULL, 0);
	sem_close(s);	
	sem_unlink("fork1");
	return (0);
}
