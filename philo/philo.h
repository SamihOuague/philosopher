/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 19:55:00 by  souaguen         #+#    #+#             */
/*   Updated: 2024/02/16 06:42:01 by souaguen         ###   ########.fr       */
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

typedef	struct s_list
{
	void		*content;
	struct s_list	*next;
}	t_list;

typedef struct	s_fork
{
	pthread_mutex_t	mut;
	int	free_fork;
}	t_fork;

typedef struct s_thread_info
{	
	pthread_t	thread;	
	unsigned int	id;
	unsigned int	time_to_sleep;
	unsigned int	time_to_eat;
	unsigned int	time_to_die;
	unsigned int	n_fork;
	unsigned int	eat_n;
	unsigned long	last_meal;
	unsigned long	*started_at;
	unsigned int	n_time_eat;
	int		*deadbeef;
	pthread_mutex_t	*locked;	
	pthread_mutex_t	*msg_lock;
	pthread_mutex_t	*meal_lock;
	t_fork		*forks;
	t_list		**msg_queue;
}	t_thread_info;

/*typedef struct s_monitor
{
	t_list		**msg_queue;
	pthread_t	thread;
	t_thread_info	*philos;
	pthread_mutex_t	*locked;	
	pthread_mutex_t	*msg_lock;
	pthread_mutex_t	*meal_lock;
	unsigned int	n_philo;
}	t_monitor;
*/
typedef struct s_msg
{
	unsigned int	id;
	unsigned int	status;
	unsigned long	timestamp;
}	t_msg;

unsigned long	get_timestamp_ms();
void		*fork_init(unsigned int n_philo);
void		*philo_info_init(int argc, char **argv);
void		ft_putchar_fd(char *str, int fd);
void		free_forks(t_fork *forks, int n_forks);
int		ft_atoi(char *str);
#endif
