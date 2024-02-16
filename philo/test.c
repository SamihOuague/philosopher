/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 06:36:43 by souaguen          #+#    #+#             */
/*   Updated: 2024/02/16 07:10:29 by souaguen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

typedef struct s_philo
{
	pthread_t	thread;
	unsigned int	id;
	pthread_mutex_t	*msg_lock;
	t_list		**msg_queue;
}	t_philo;

typedef struct s_monitor
{
	pthread_t	thread;	
	pthread_mutex_t	*msg_lock;
	t_list		**msg_queue;
}	t_monitor;

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

void	*routine(void *arg)
{
	t_philo	*self;
	int	*id;

	self = arg;
	while (1)
	{
		pthread_mutex_lock((*self).msg_lock);
		id = malloc(sizeof(int));
		if (id == NULL)
			return (NULL);
		*id = (int)(*self).id;
		ft_lstadd_back((*self).msg_queue, ft_lstnew(id));
		pthread_mutex_unlock((*self).msg_lock);
		usleep(800000);
	}
	return (NULL);
}

void	*monitor_routine(void *arg)
{
	t_monitor	*self;

	self = arg;
	while (1)
	{
		while (*(*self).msg_queue != NULL)
		{
			pthread_mutex_lock((*self).msg_lock);
			printf("%d\n", *(int *)(**(*self).msg_queue).content);
			ft_pop((*self).msg_queue);
			pthread_mutex_unlock((*self).msg_lock);
		}
		usleep(1000);
	}
	return (NULL);
}

int	main(int argc, char **argv)
{	
	pthread_mutex_t	msg_lock;
	t_monitor	monitor;
	t_philo		philo[2];
	t_list		*queue;
	int		i;

	i = -1;
	queue = NULL;
	pthread_mutex_init(&msg_lock, NULL);
	while ((++i) < 2)
	{
		philo[i].id = i + 1;
		philo[i].msg_queue = &queue;
		philo[i].msg_lock = &msg_lock;
		pthread_create(&philo[i].thread, NULL, &routine, &philo[i]);
	}
	monitor.msg_lock = &msg_lock;
	monitor.msg_queue = &queue;
	pthread_create(&monitor.thread, NULL, &monitor_routine, &monitor);
	i = -1;
	while ((++i) < 2)
		pthread_join(philo[i].thread, NULL);
	pthread_join(monitor.thread, NULL);
	pthread_mutex_destroy(&msg_lock);
	return (0);
}
