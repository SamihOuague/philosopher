/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 22:13:26 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/20 09:49:11 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	precision_sleep(int time_to_sleep)
{
	unsigned long	checkpoint;

	checkpoint = get_timestamp_ms();
	while ((get_timestamp_ms() - checkpoint) < time_to_sleep)
		usleep(100);
}

t_list	*ft_lstlast(t_list *lst)
{
	t_list	*cursor;

	cursor = lst;
	while ((*cursor).next != NULL)
		cursor = (*cursor).next;
	return (cursor);
}

t_list	*ft_lstnew(void *content)
{
	t_list	*lst;
	
	lst = malloc(sizeof(t_list));
	if (lst == NULL)
		return (NULL);
	(*lst).content = content;
	(*lst).next = NULL;
	return (lst);
}

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*last;

	if (*lst == NULL)
	{
		*lst = new;
		return ;
	}
	last = ft_lstlast(*lst);
	(*last).next = new;
}

t_list	*ft_pop(t_list **lst)
{
	t_list	*tmp;

	if (lst == NULL || *lst == NULL)
		return (NULL);
	tmp = *lst;
	*lst = (**lst).next;
	(*tmp).next = NULL;
	return (tmp);
}

int	is_numeric(char *str)
{
	int	i;

	i = -1;
	while (*(str + (++i)) != '\0')
	{
		if (*(str + i) > '9' || *(str + i) < '0')
			return (0);
	}
	return (1);
}

int	*check_args(int argc, char **argv)
{
	int	i;
	int	*args;

	
	i = 0;
	args = malloc(sizeof(int) * 5);
	args[4] = -1;
	while ((++i) < argc)
	{
		args[i - 1] = ft_atoi(argv[i]);
		if (!is_numeric(argv[i]) || (args[i - 1] >= 1000000 || args[i - 1] <= 0))
		{
			free(args);
			args = NULL;
			return (NULL);
		}
	}
	return (args);
}

void	free_forks(t_fork *forks, int n_forks)
{
	int	i;
	
	i = -1;
	while ((++i) < n_forks)
		pthread_mutex_destroy(&forks[i].mut);
	free(forks);
}

t_fork	*init_forks(unsigned int n_philo)
{
	int	i;
	t_fork	*forks;

	i = -1;
	forks = malloc(sizeof(t_fork) * n_philo);
	if (forks == NULL)
		return (NULL);
	while ((++i) < n_philo)
	{
		forks[i].free_fork = 0;
		pthread_mutex_init(&forks[i].p_mut, NULL);
		pthread_mutex_init(&forks[i].mut, NULL);
	}
	return (forks);
}

t_philo	*init_philo(int *args)
{
	t_philo	*philo;
	t_fork	*forks;
	int		i;

	philo = malloc(sizeof(t_philo) * args[0]);
	if (philo == NULL)
		return (NULL);
	forks = init_forks(args[0]);
	if (forks == NULL)
	{
		free(philo);
		return (NULL);
	}
	i = -1;
	while ((++i) < args[0])
	{
		philo[i].id = i + 1;
		philo[i].time_to_die = args[1];
		philo[i].time_to_eat = args[2];
		philo[i].time_to_sleep = args[3];
		philo[i].n_time_eat = args[4];
		philo[i].n_fork = args[0];
		philo[i].forks = forks;
	}
	return (philo);
}

void	send_msg(t_philo *self, int status)
{
	t_msg	*msg;

	msg = malloc(sizeof(t_msg));
	if (msg == NULL)
		return ;
	(*msg).id = (*self).id;
	(*msg).status = status;	
	pthread_mutex_lock((*self).msg_lock);
	(*msg).timestamp = get_timestamp_ms();
	ft_lstadd_back((*self).msg_queue, ft_lstnew(msg));
	pthread_mutex_unlock((*self).msg_lock);
}

void	*monitor_routine(void *arg)
{
	unsigned long	*last_meal;
	unsigned long	t;
	int		*n_time_eat;
	t_monitor	*self;
	t_msg		*msg;
	t_list		*tmp;
	char		*str;
	int		i;
	int		started_at;



	i = -1;	
	self = arg;
	msg = NULL;
	started_at = (*(*self).philos).started_at;
	t = get_timestamp_ms();
	n_time_eat = malloc(sizeof(int) * (*self).n_philo);
	last_meal = malloc(sizeof(unsigned long) * (*self).n_philo);
	while ((++i) < (*self).n_philo)
	{
		n_time_eat[i] = 0;
		last_meal[i] = t;
	}
	while (1)
	{
		usleep(100);
		pthread_mutex_lock((*self).msg_lock);
		while (*(*self).msg_queue != NULL)
		{
			tmp = ft_pop((*self).msg_queue);
			msg = (*tmp).content;
			if ((*msg).status == 0)
				str = "has taken a fork";
			else if ((*msg).status == 1)
				str = "is thinking";
			else if ((*msg).status == 2)
			{
				last_meal[(*msg).id - 1] = (*msg).timestamp;
				n_time_eat += 1;
				str = "is eating";
			}
			else if ((*msg).status == 3)
				str = "is sleeping";
			printf("%ld %d %s\n", (*msg).timestamp - (*self).philos[0].started_at, (*msg).id, str);
			free(msg);
			free(tmp);
		}
		pthread_mutex_unlock((*self).msg_lock);
		t = get_timestamp_ms();
		i = -1;
		while ((++i) < (*self).n_philo)
		{
			if ((t - last_meal[i]) > (*self).philos[i].time_to_die 
				&& (*self).philos[i].n_time_eat != n_time_eat[i])
			{
				pthread_mutex_lock((*self).msg_lock);
				printf("%ld %d died\n", t - (*self).philos[i].started_at, i + 1);
				pthread_mutex_unlock((*self).msg_lock);
				return (NULL);
			}
		}
	}
	return (NULL);
}

void	*philo_routine(void *arg)
{
	t_philo	*self;
	int	current_fork;
	int	next_fork;
	unsigned long	checkpoint;
	
	self = arg;
	current_fork = (*self).id - 1;
	next_fork = (*self).id % (*self).n_fork;
	if (next_fork < current_fork)
	{
		current_fork = next_fork;
		next_fork = (*self).id - 1;
	}
	while (1)
	{
		send_msg(self, 1);
		pthread_mutex_lock(&(*self).forks[current_fork].mut);
		send_msg(self, 0);
		pthread_mutex_lock(&(*self).forks[next_fork].mut);
		send_msg(self, 0);	
		checkpoint = get_timestamp_ms();
		send_msg(self, 2);
		precision_sleep((*self).time_to_eat - (get_timestamp_ms() - checkpoint));	
		pthread_mutex_unlock(&(*self).forks[current_fork].mut);
		pthread_mutex_unlock(&(*self).forks[next_fork].mut);
		checkpoint = get_timestamp_ms();
		send_msg(self, 3);
		precision_sleep((*self).time_to_sleep - (get_timestamp_ms() - checkpoint));
	}
	return (NULL);
}

void	run(t_philo *philo)
{
	pthread_mutex_t	locked;
	pthread_mutex_t	msg_lock;
	t_list		*msg_queue;
	unsigned long	started_at;
	int		counter;
	t_monitor	monitor;
	int			i;

	if (philo == NULL)
		return ;
	i = -1;
	counter = 0;
	msg_queue = NULL;
	pthread_mutex_init(&locked, NULL);
	pthread_mutex_init(&msg_lock, NULL);
	started_at = get_timestamp_ms();	
	pthread_mutex_lock(&locked);
	while ((++i) < (*philo).n_fork)
	{
		philo[i].started_at = started_at;
		philo[i].msg_lock = &msg_lock;
		philo[i].locked = &locked;
		philo[i].msg_queue = &msg_queue;
		philo[i].counter = &counter;
		pthread_mutex_init(&philo[i].meal_lock, NULL);
		pthread_create(&philo[i].thread, NULL, &philo_routine, &philo[i]);
	}
	monitor.msg_queue = &msg_queue;
	monitor.msg_lock = &msg_lock;
	monitor.n_philo = (*philo).n_fork;
	monitor.philos = philo;
	pthread_create(&monitor.thread, NULL, &monitor_routine, &monitor);
	pthread_mutex_unlock(&locked);
	i = -1;
	while ((++i) < (*philo).n_fork)
		pthread_join(philo[i].thread, NULL);
	pthread_join(monitor.thread, NULL);
}

int	main(int argc, char **argv)
{
	int	*args;

	args = NULL;
	if (argc < 5 || argc > 6)
	{
		ft_putstr_fd("Number of arguments must be 5 or 6\n", 2);
		return (1);
	}
	args = check_args(argc, argv);
	if (args == NULL)
	{
		ft_putstr_fd("Invalid argument !\n", 2);
		return (1);
	}
	run(init_philo(args));
	free(args);
	return (0);
}
