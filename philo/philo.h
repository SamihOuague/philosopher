/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 19:55:00 by  souaguen         #+#    #+#             */
/*   Updated: 2024/02/13 07:30:02 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <stdio.h>
# include <pthread.h>
# include <unistd.h>

typedef struct s_fork
{
	pthread_mutex_t	mut;
}	t_fork;

typedef struct s_thread_info
{
	unsigned int	id;
	pthread_t	thread;	
	unsigned int	time_to_sleep;
	unsigned int	time_to_eat;
	unsigned int	time_to_die;
	unsigned int	n_fork;
	pthread_mutex_t	*meal_lock;
	unsigned long	last_meal;
	unsigned long	*started_at;
	unsigned int	n_time_eat;
	int		*deadbeef;
	int		*started;
	pthread_mutex_t	*locked;
	t_fork		*forks;
}	t_thread_info;

unsigned long	get_timestamp_ms();
void		*fork_init(unsigned int n_philo);
void		*philo_info_init(int argc, char **argv);
void		ft_putchar_fd(char *str, int fd);
void		free_forks(t_fork *forks, int n_forks);
int		ft_atoi(char *str);
#endif
