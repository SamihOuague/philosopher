/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 06:36:43 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/17 19:41:09 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

typedef struct s_philo
{
	pthread_t	thread;
	unsigned int	id;
	unsigned int	time_to_sleep;
	unsigned int	time_to_die;
	unsigned int	time_to_eat;
	unsigned int	n_fork;
	unsigned long	*started_at;
	unsigned long	last_meal;
	int		n_time_eat;
	pthread_mutex_t	*msg_lock;
	pthread_mutex_t	meal_lock;
	pthread_mutex_t *dead_lock;
	t_list		**msg_queue;
	t_list		**priority_queue;
	t_fork		*forks;
	int		*deadbeef;
}	t_philo;

typedef struct s_monitor
{
	pthread_t	thread;
	pthread_mutex_t	*msg_lock;
	t_list		**msg_queue;
	t_philo		*philos;
	pthread_mutex_t *dead_lock;
	int		*deadbeef;
}	t_monitor;

void    precision_sleep(unsigned int *time_to_sleep)
{
        unsigned long   checkpoint;

        checkpoint = get_timestamp_ms();
        while ((get_timestamp_ms() - checkpoint) < (*time_to_sleep))
                usleep(1000);
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

void	ft_pop(t_list **lst)
{
	t_list	*tmp;

	if (lst == NULL || *lst == NULL)
		return ;
	tmp = (**lst).next;
	free(*lst);
	*lst = tmp;
}

t_list	*ft_lstlast(t_list *lst)
{
	t_list	*cursor;

	cursor = lst;
	while ((*cursor).next != NULL)
		cursor = (*cursor).next;
	return (cursor);
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

t_fork	*init_fork(unsigned int n_fork)
{
	t_fork	*forks;
	int		i;

	i = -1;
	forks = malloc(sizeof(t_fork) * n_fork);
	if (forks == NULL)
		return (NULL);
	while ((++i) < n_fork)
		pthread_mutex_init(&forks[i].mut, NULL);
	return (forks);
}

void	send_msg(t_philo *self, int status)
{
	t_msg	*msg;

	pthread_mutex_lock((*self).msg_lock);	
	msg = malloc(sizeof(t_msg));
	if (msg == NULL)
		return ;
	(*msg).status = status;
	(*msg).id = (*self).id;
	(*msg).timestamp = get_timestamp_ms() - *(*self).started_at;	
	ft_lstadd_back((*self).msg_queue, ft_lstnew(msg));
	pthread_mutex_unlock((*self).msg_lock);
}

void	*routine(void *arg)
{
	t_philo	*self;
	t_fork	*forks;
	unsigned int	checkpoint;
	int		current_fork;
	int		next_fork;

	self = arg;
	forks = (*self).forks;
	current_fork = (*self).id - 1;
	next_fork = (*self).id % (*self).n_fork;
	pthread_mutex_lock(&(*self).meal_lock);
	(*self).last_meal = get_timestamp_ms();
	pthread_mutex_unlock(&(*self).meal_lock);
	if ((*self).id % 2 == 0)
	{
		current_fork = next_fork;
		next_fork = (*self).id - 1;
	}
	while (1)
	{
		pthread_mutex_lock((*self).dead_lock);
		if (*(*self).deadbeef == 1)
		{
			pthread_mutex_unlock((*self).dead_lock);
			break ;
		}
		pthread_mutex_unlock((*self).dead_lock);
		ft_lstadd_back((*self).priority_queue, ft_lstnew(&(*self).id));
		send_msg(self, 0);
		
		while (*(int *)(**(*self).priority_queue).content != (*self).id)
			usleep(1000);
		
		pthread_mutex_lock(&forks[current_fork].mut);	
		send_msg(self, 2);
		pthread_mutex_lock(&forks[next_fork].mut);
		send_msg(self, 2);
		ft_pop((*self).priority_queue);
		pthread_mutex_lock(&(*self).meal_lock);
		(*self).last_meal = get_timestamp_ms();
		pthread_mutex_unlock(&(*self).meal_lock);
		checkpoint = (*self).time_to_eat;
		pthread_mutex_lock(&(*self).meal_lock);
		(*self).last_meal = get_timestamp_ms();
		pthread_mutex_unlock(&(*self).meal_lock);
		send_msg(self, 1);
		precision_sleep(&checkpoint);	
		send_msg(self, 3);
		pthread_mutex_unlock(&forks[current_fork].mut);
		pthread_mutex_unlock(&forks[next_fork].mut);
		checkpoint = (*self).time_to_sleep; 
		precision_sleep(&checkpoint);
	}
	
	return (NULL);
}

int	philo_dead(t_philo *philo)
{
	int	i;

	i = -1;
	while ((++i) < (*philo).n_fork)
	{
		pthread_mutex_lock(&philo[i].meal_lock);
		if ((get_timestamp_ms() - philo[i].last_meal) > philo[i].time_to_die)
		{
			pthread_mutex_unlock(&philo[i].meal_lock);
			return (i);
		}
		pthread_mutex_unlock(&philo[i].meal_lock);
	}
	return (-1);
}

void	*monitor_routine(void *arg)
{
	t_monitor	*self;
	t_msg		*msg;
	char		*str;
	int		index;

	index = -1;
	self = arg;
	while (1)
	{
		index = philo_dead((*self).philos);
		pthread_mutex_lock((*self).msg_lock);
		while (*(*self).msg_queue != NULL)
		{
			index = philo_dead((*self).philos);
			if (index != -1)
				break ;	
			msg = (t_msg *)(**(*self).msg_queue).content;
			if ((*msg).status == 0)
				str = "is thinking";
			else if ((*msg).status == 1)
				str = "is eating";
			else if ((*msg).status == 2)
				str = "has taken a fork";
			else if ((*msg).status == 3)
				str = "is sleeping";
			printf("%ld %d %s\n", (*msg).timestamp, (*msg).id, str);
			ft_pop((*self).msg_queue);
		}
		if (index != -1)
		{
			printf("%ld %d died\n", get_timestamp_ms() - *(*self).philos[index].started_at, index + 1);
			pthread_mutex_unlock((*self).msg_lock);
			pthread_mutex_lock((*self).dead_lock);
			*(*self).deadbeef = 1;
			pthread_mutex_unlock((*self).dead_lock);
			break ;
		}
		pthread_mutex_unlock((*self).msg_lock);	
		usleep(1000);
	}
	return (NULL);
}

t_philo	*init_properties(int *args)
{	
	t_philo		*philo;
	int		i;	

	i = -1;
	philo = malloc(sizeof(t_philo) * args[0]);
	if (philo == NULL)
		return (NULL);
	while ((++i) < args[0])
	{
		philo[i].id = i + 1;
		philo[i].n_fork = args[0];
		philo[i].time_to_die = args[1];
		philo[i].time_to_eat = args[2];
		philo[i].time_to_sleep = args[3];
		philo[i].n_time_eat = args[4];
	}
	return (philo);
}

t_philo	*init_philo(int argc, char **argv)
{
	int		args[5];
	int		i;

	i = -1;
	args[4] = -1;
	while ((++i) < 4)
	{
		args[i] = ft_atoi(argv[i + 1]);
		if (args[i] <= 0 || args[i] >= 1000000)
			return (NULL);
	}
	if (argc == 6)
	{
		args[4] = ft_atoi(argv[5]);
		if (ft_atoi(argv[5]) <= 0 || ft_atoi(argv[5]) >= 1000000)
			return (NULL);
	}
	return (init_properties(args));
}

void	run(t_philo *philo)
{
	pthread_mutex_t	msg_lock;
	pthread_mutex_t	dead_lock;
	pthread_mutex_t	meal_lock;
	t_monitor	monitor;
	t_list		*queue;
	t_list		*p_queue;
	unsigned long	started_at;
	t_fork		*forks;
	int		deadbeef;
	int		i;

	i = -1;	
	deadbeef = 0;
	forks = init_fork((*philo).n_fork);
	queue = NULL;
	p_queue = NULL;
	pthread_mutex_init(&msg_lock, NULL);
	pthread_mutex_init(&dead_lock, NULL);
	pthread_mutex_init(&meal_lock, NULL);
	started_at = get_timestamp_ms();
	while ((++i) < (*philo).n_fork)
	{
		philo[i].forks = forks;
		philo[i].msg_queue = &queue;	
		philo[i].priority_queue = &p_queue;
		philo[i].msg_lock = &msg_lock;
		philo[i].started_at = &started_at;
		philo[i].deadbeef = &deadbeef;
		philo[i].dead_lock = &dead_lock;
		pthread_mutex_init(&philo[i].meal_lock, NULL);
		pthread_create(&philo[i].thread, NULL, &routine, &philo[i]);
	}
	i = -1;
	monitor.msg_queue = &queue;
	monitor.msg_lock = &msg_lock;
	monitor.deadbeef = &deadbeef;
	monitor.dead_lock = &dead_lock;
	monitor.philos = philo;
	pthread_create(&monitor.thread, NULL, &monitor_routine, &monitor);
	while ((++i) < (*philo).n_fork)
		pthread_join(philo[i].thread, NULL);
	pthread_join(monitor.thread, NULL);
}

int	main(int argc, char **argv)
{
	t_philo	*philo;
	
	philo = NULL;
	if (!(argc == 5 || argc == 6))
		return (1);
	philo = init_philo(argc, argv);
	run(philo);	
	free(philo);
	return (0);
}
