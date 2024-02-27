/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 19:55:00 by  souaguen         #+#    #+#             */
/*   Updated: 2024/02/27 05:53:13 by souaguen         ###   ########.fr       */
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

typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}	t_list;

typedef struct s_fork
{
	pthread_mutex_t	mut;
	pthread_mutex_t	p_mut;
	int				free_fork;
}	t_fork;

typedef struct s_philo
{	
	pthread_t		thread;	
	unsigned int	id;
	unsigned int	time_to_sleep;
	unsigned int	time_to_eat;
	unsigned int	time_to_die;
	unsigned int	n_fork;
	unsigned int	eat_n;
	unsigned long	last_meal;
	unsigned long	started_at;
	unsigned int	n_time_eat;
	pthread_mutex_t	*locked;
	pthread_mutex_t	*msg_lock;
	pthread_mutex_t	meal_lock;
	t_fork			*forks;
	t_list			**msg_queue;
	int				*deadbeef;
	int				*counter;
}	t_philo;

typedef struct s_monitor
{
	pthread_t		thread;
	pthread_mutex_t	*locked;
	pthread_mutex_t	*msg_lock;
	pthread_mutex_t	*meal_lock;
	t_philo			*philos;	
	t_list			**msg_queue;	
	unsigned long	started_at;
	unsigned int	n_philo;	
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
	unsigned long	started_at;
	pthread_mutex_t	locked;
	pthread_mutex_t	msg_lock;
	t_list			*msg_queue;
	int				deadbeef;
}	t_shared;

unsigned long	get_timestamp_ms(void);
void			precision_sleep(int time_to_sleep);
void			ft_putstr_fd(char *str, int fd);
void			free_forks(t_fork *forks, int n_forks);
void			ft_lstadd_back(t_list **lst, t_list *new);
void			send_msg(t_philo *self, int status);
void			take_forks(t_philo *self);
void			get_meal(t_philo *self);
void			get_sleep(t_philo *self);
void			*philo_routine(void *arg);
void			create_philo_thread(t_philo *philo, t_shared *shared);
void			print_msgs(t_monitor *s, unsigned long *lm, int *nt_eat);
void			*monitor_routine(void *arg);
void			create_mon_thread(t_monitor *m, t_philo *p, t_shared *shr);
void			init_track(unsigned long **lm, int **nt_eat, int siz);
char			*get_msg(t_msg *msg, unsigned long *last_meal, int *n_time_eat);
t_philo			*init_philo(int *args);
t_fork			*init_forks(unsigned int n_philo);
t_list			*ft_pop(t_list **lst);
t_list			*ft_lstlast(t_list *lst);
t_list			*ft_lstnew(void *content);
int				check_death(t_monitor *s, unsigned long *lm, int *nt_eat);
int				*check_args(int argc, char **argv);
int				is_numeric(char *str);
int				ft_atoi(char *str);
#endif
