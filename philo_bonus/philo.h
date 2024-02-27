/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 06:35:03 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/27 21:20:09 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H
# include <fcntl.h>
# include <sys/stat.h>
# include <semaphore.h>
# include <stdio.h>
# include <sys/types.h>
# include <unistd.h>
# include <sys/time.h>
# include <sys/wait.h>
# include <stdlib.h>
# include <string.h>
# include <pthread.h>

typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}	t_list;

typedef struct s_philo
{
	unsigned long	last_meal;
	unsigned long	started_at;
	pid_t			pid;
	sem_t			*forks;
	sem_t			*msg_lock;
	sem_t			*fork_lock;
	sem_t			*die_lock;
	t_list			**msg_queue;
	int				id;
	int				n_fork;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				n_time_eat;
	int				*args;
}	t_philo;

typedef struct s_monitor
{
	unsigned long	started_at;
	unsigned int	n_philo;
	pthread_t		thread;
	sem_t			*msg_lock;
	t_philo			*philos;
	t_list			**msg_queue;	
	int				*deadbeef;
}	t_monitor;

typedef struct s_msg
{
	unsigned int	id;
	unsigned int	status;
	unsigned long	timestamp;
}	t_msg;

typedef struct s_shared
{
	sem_t			*forks;
	sem_t			*msg_sem;
	sem_t			*fork_sem;
	sem_t			*die_sem;
}	t_shared;

unsigned long	get_timestamp_ms(void);
t_philo			*init_philo(int *args);
void			ft_putstr_fd(char *str, int fd);
void			send_msg(t_philo *self, int status);
void			clean_mem_prog(t_philo *self, t_philo *philo, int n_eat);
void			init_sem(t_shared *shared, int n_fork);
void			init_philo_sem(t_philo *p, t_shared shrd,
					unsigned long s_at, int n);
void			clear_parent_sem(t_shared shared);
int				*check_args(int argc, char **argv);
int				precision_sleep(int time_to_sleep, t_philo *self);
int				ft_strcpy(char *dest, char *src);
int				is_numeric(char *str);
int				ft_atoi(char *str);
int				wait_forks(t_philo *self);
int				philo_proc(t_philo *philo, int index);
int				wait_for_philos(t_shared shared, t_philo *philo, int *args);
int				philo_proc(t_philo *philo, int index);
int				philo_routine(t_philo *self);
int				go_to_sleep(t_philo *self);
#endif
