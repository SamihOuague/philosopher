/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 04:17:54 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/16 06:32:25 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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

void	ft_lstclear(t_list **lst, void (del)(void *))
{
	t_list	*cursor;
	t_list	*tmp;

	cursor = *lst;
	while (cursor != NULL)
	{
		tmp = (*cursor).next;
		del((*cursor).content);
		free(cursor);
		cursor = tmp;
	}
}

void	precision_sleep(unsigned int *time_to_sleep)
{
	unsigned long	checkpoint;

	checkpoint = get_timestamp_ms(); 
	while ((get_timestamp_ms() - checkpoint) < (*time_to_sleep))
		usleep(1000);
}

void	locked_printf(char *format, t_thread_info *self)
{
	pthread_mutex_lock((*self).locked);
	if (*(*self).deadbeef == 1)
	{
		pthread_mutex_unlock((*self).locked);
		return ;
	}
	printf(format, get_timestamp_ms() - *(*self).started_at, (*self).id);
	pthread_mutex_unlock((*self).locked);
}

void	*start_routine(void *arg)
{
	t_thread_info	*self;
	t_list		*queue;
	int		*id;

	self = arg;
	pthread_mutex_lock((*self).msg_lock);
	queue = *(*self).msg_queue;
	id = malloc(sizeof(int));
	*id = (*self).id;
	ft_lstadd_back((*self).msg_queue, ft_lstnew(id));
	pthread_mutex_unlock((*self).msg_lock);
	return (NULL);
}

void	*monitor_routine(void *arg)
{
	t_monitor	*self;
	t_list		*queue;
	t_list		*cursor;

	self = arg;
	pthread_mutex_lock((*self).msg_lock);
	pthread_mutex_unlock((*self).msg_lock);
	while (1)
	{
		pthread_mutex_lock((*self).msg_lock);
		cursor = queue;
		while (*(*self).msg_queue != NULL)
		{
			printf("%d send msg\n", *(int *)(**(*self).msg_queue).content);
			pthread_mutex_lock((*self).locked);
		}
		pthread_mutex_unlock((*self).msg_lock);
		usleep(1000);
	}
	return (NULL);
}

void	run(t_thread_info *philo, unsigned int n_philo)
{
	pthread_mutex_t	msg_lock;
	pthread_mutex_t	meal_lock;
	pthread_mutex_t	locked;
	t_list		*msg_queue;
	t_monitor	monitor;
	unsigned long	started_at;
	int		i;

	i = -1;	
	msg_queue = NULL;
	started_at = get_timestamp_ms();
	pthread_mutex_init(&locked, NULL);
	pthread_mutex_init(&msg_lock, NULL);
	pthread_mutex_init(&meal_lock, NULL);
	while ((++i) < n_philo)
	{
		philo[i].id = i + 1;
		philo[i].msg_lock = &msg_lock;
		philo[i].meal_lock = &meal_lock;
		philo[i].locked = &locked;
		philo[i].msg_queue = &msg_queue;	
		philo[i].started_at = &started_at;
		pthread_create(&philo[i].thread, NULL, &start_routine, &philo[i]);
	}
	monitor.msg_queue = &msg_queue;
	monitor.philos = philo;
	monitor.n_philo = (*philo).n_fork;
	monitor.meal_lock = &meal_lock;	
	monitor.locked = &locked;
	monitor.msg_queue = &msg_queue;
	pthread_create(&monitor.thread, NULL, &monitor_routine, &monitor);	
	i = -1;
	while ((++i) < n_philo)
		pthread_join(philo[i].thread, NULL);
	pthread_join(monitor.thread, NULL);
}


int	main(int argc, char **argv)
{
	int		n_philo;
	t_thread_info	*philo;

	if (!(argc == 5 || argc == 6))
		return (1);
	n_philo = ft_atoi(argv[1]);
	philo = philo_info_init(argc, argv);
	if (philo == NULL)
		return (1);
	run(philo, n_philo);
//	free_forks((*philo).forks, n_philo);
//	free(philo);
	return (0);
}
