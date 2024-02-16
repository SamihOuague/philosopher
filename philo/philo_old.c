/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_old.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: souaguen <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 00:48:28 by  souaguen         #+#    #+#             */
/*   Updated: 2024/02/16 04:45:32 by souaguen         ###   ########.fr       */
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

void	take_forks(t_thread_info *self)
{
	t_fork		*forks;
	t_list		*queue;
	unsigned long	checkpoint;
	int		current_fork;
	int		next_fork;
	int		break_loop;

	forks = (*self).forks;
	current_fork = (*self).id - 1;
	next_fork = (*self).id % (*self).n_fork;
	if (((*self).id % 2) == 0)
	{
		current_fork = next_fork;
		next_fork = (*self).id - 1;
	}
	
	pthread_mutex_lock(&(*(*self).status).think_mut);
	queue = *(*self).queue;
	pthread_mutex_unlock(&(*(*self).status).think_mut);
	while (1)
	{
		pthread_mutex_lock((*self).locked);
		if (*(*self).deadbeef == 1)
		{
			pthread_mutex_unlock((*self).locked);
			return ;
		}
		pthread_mutex_unlock((*self).locked);
		pthread_mutex_lock(&(*(*self).status).think_mut);
		if (queue != NULL && (*self).id == *(int *)(**(*self).queue).content)
		{	
			pthread_mutex_unlock(&(*(*self).status).think_mut);
			break ;
		}
		pthread_mutex_unlock(&(*(*self).status).think_mut);
		usleep(100);
	}	
	pthread_mutex_lock(&forks[current_fork].mut);
	locked_printf("%ld %d has taken a fork\n", self);
	pthread_mutex_lock(&forks[next_fork].mut);
	locked_printf("%ld %d has taken a fork\n", self);
	pthread_mutex_lock(&(*(*self).status).think_mut);
	ft_pop((*self).queue);
	pthread_mutex_unlock(&(*(*self).status).think_mut);
}

void	start_eating(t_thread_info *self)
{
	t_fork		*forks;
	int		current_fork;
	int		next_fork;
	
	forks = (*self).forks;
	current_fork = (*self).id - 1;
	next_fork = (*self).id % (*self).n_fork;
	
	locked_printf("%ld %d is eating\n", self);
	pthread_mutex_lock((*self).meal_lock);
	(*self).last_meal = get_timestamp_ms();
	pthread_mutex_unlock((*self).meal_lock);	
	precision_sleep(&(*self).time_to_eat);
	if (((*self).id % 2) == 0)
	{
		current_fork = next_fork;
		next_fork = (*self).id - 1;
	}
	pthread_mutex_unlock(&forks[current_fork].mut);	
	pthread_mutex_unlock(&forks[next_fork].mut);
}

void	start_sleeping(t_thread_info *self)
{
	unsigned int	time_to_sleep;

	time_to_sleep = (*self).time_to_sleep;
	locked_printf("%ld %d is sleeping\n", self);
	precision_sleep(&time_to_sleep);
}

void	philo_dead(t_thread_info *self)
{

	pthread_mutex_lock((*self).locked);
	if (*(*self).deadbeef == 1)
	{
		pthread_mutex_unlock((*self).locked);
		return ;
	}
	if ((*self).eat_n != (*self).n_time_eat)
	{
		*(*self).deadbeef = 1;
		printf("%ld %d died\n", get_timestamp_ms() - *(*self).started_at, (*self).id);
	}
	pthread_mutex_unlock((*self).locked);
}

int	dead_philo(t_thread_info *philo, pthread_mutex_t *locked)
{
	int	i;
	int	n_philo;
	unsigned long	checkpoint;

	i = 0;		
	pthread_mutex_lock(locked);	
	checkpoint = get_timestamp_ms() - philo[i].last_meal;	
	pthread_mutex_unlock(locked);
	n_philo = philo[0].n_fork;
	while (i < n_philo)
	{
		if (checkpoint > 0 && checkpoint > philo[i].time_to_die)
		{
			pthread_mutex_unlock(locked);
			return (i);
		}
		i++;
	}
	return(-1);
}

void	*dead_monitor(void *arg)
{
	t_thread_info	*philo;
	pthread_mutex_t	*meal_lock;
	pthread_mutex_t	*locked;
	int	index;

	philo = arg;
	locked = (*philo).locked;
	meal_lock = (*philo).meal_lock;
	while (1)
	{
		index = dead_philo(philo, meal_lock);
		if (index != -1)
		{
			philo_dead(&philo[index]);
			break ;
		}
		usleep(500);
	}
	return (NULL);
}

void	*start_routine(void *arg)
{
	t_thread_info *self;
	long	checkpoint;
	unsigned long	start;
	int		i;
	t_fork		*forks;
	t_list		*queue;

	i = 0;
	self = arg;
	forks = (*self).forks;
	start = *(*self).started_at;
	

	pthread_mutex_lock(&(*(*self).status).think_mut);
	queue = *(*self).queue;	
	pthread_mutex_unlock(&(*(*self).status).think_mut);
	if ((*self).n_fork <= 1)
	{
		pthread_mutex_lock(&(*self).forks[0].mut);
		locked_printf("%ld %d has taken a fork\n", self);
		locked_printf("%ld %d is thinking\n", self);
		usleep((*self).time_to_die * 1000);
		pthread_mutex_unlock(&(*self).forks[0].mut);
		return (NULL);
	}
	pthread_mutex_lock((*self).meal_lock);
	(*self).last_meal = get_timestamp_ms();
	pthread_mutex_unlock((*self).meal_lock);
	while ((*self).eat_n != (*self).n_time_eat)
	{
		pthread_mutex_lock(&(*(*self).status).think_mut);
		ft_lstadd_back((*self).queue, ft_lstnew(&(*self).id));
		pthread_mutex_unlock(&(*(*self).status).think_mut);
		locked_printf("%ld %d is thinking\n", self);
		take_forks(self);
		start_eating(self);
		pthread_mutex_lock((*self).locked);
		(*self).eat_n += 1;
		pthread_mutex_unlock((*self).locked);
		start_sleeping(self);
		pthread_mutex_lock((*self).locked);
		if (*(*self).deadbeef == 1)
		{
			pthread_mutex_unlock((*self).locked);
			break ;
		}
		pthread_mutex_unlock((*self).locked);
	}
	return (NULL);
}

t_status	*init_status(t_status *status)
{
	(*status).eat_status = 0;
	(*status).sleep_status = 0;
	(*status).think_status = 0;
	pthread_mutex_init(&(*status).eat_mut, NULL);
	pthread_mutex_init(&(*status).think_mut, NULL);
	pthread_mutex_init(&(*status).sleep_mut, NULL);
	return (status);
}

void	run(t_thread_info *philo, int n_philo)
{
	int		i;
	unsigned long	start;
	pthread_mutex_t	locked;
	pthread_mutex_t	meal_lock;
	pthread_mutex_t	forks_lock;	
	t_status	status;
	int		deadbeef;
	int		started;
	pthread_t	monitor;
	t_list		*queue;

	i = -1;	
	started = 0;	
	deadbeef = 0;
	queue = NULL;
	pthread_mutex_init(&locked, NULL);
	pthread_mutex_init(&meal_lock, NULL);
	pthread_mutex_init(&forks_lock, NULL);
	pthread_mutex_lock(&meal_lock);
	start = get_timestamp_ms();
	while ((++i) < n_philo)
	{
		philo[i].last_meal = get_timestamp_ms();
		philo[i].meal_lock = &meal_lock;
		philo[i].started_at = &start;
		philo[i].locked = &locked;
		philo[i].forks_lock = &forks_lock;
		philo[i].eat_n = 0;
		philo[i].deadbeef = &deadbeef;
		philo[i].started = &started;
		philo[i].status = init_status(&status);
		philo[i].queue = &queue;
		pthread_create(&philo[i].thread, NULL, &start_routine, &philo[i]);
	}
	pthread_mutex_unlock(&meal_lock);
	pthread_create(&monitor, NULL, &dead_monitor, philo);
	pthread_join(monitor, NULL);
	i = -1;	
	while ((++i) < n_philo)
		pthread_join(philo[i].thread, NULL);
	pthread_mutex_destroy(&locked);
	pthread_mutex_destroy(&meal_lock);
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
	free_forks((*philo).forks, n_philo);
	free(philo);
	return (0);
}
