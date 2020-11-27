/* elevator_null.c
   Null solution for the elevator threads lab.
   Jim Plank
   CS560
   Lab 2
   January, 2009
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "elevator.h"

Dllist people_list;
pthread_cond_t *holding;
pthread_cond_t *arrived;

void initialize_simulation(Elevator_Simulation *es)
{
    /* Called once at the beginning of the simulation */
    people_list = new_dllist();
    holding = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(holding, NULL);

    arrived = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(arrived, NULL);
    es->v = people_list;    //void pointer to list of people

}

void initialize_elevator(Elevator *e)
{
}

void initialize_person(Person *e)
{
}

void wait_for_elevator(Person *p)
{
    Dllist *position;
    position = (Dllist *)(p->es->v);

    pthread_mutex_lock(p->es->lock);
    dll_append(people_list, new_jval_v(p));
    pthread_cond_signal(holding);
    pthread_mutex_unlock(p->es->lock);

    pthread_mutex_lock(p->lock);
    pthread_cond_wait(p->cond, p->lock);
    pthread_mutex_unlock(p->lock);
}

void wait_to_get_off_elevator(Person *p)
{
    //signal elevator
    Elevator *el = p->e;
    pthread_mutex_lock(el->lock);
    pthread_cond_signal(el->cond);
    pthread_mutex_unlock(el->lock);

    //block person
    pthread_mutex_lock(p->lock);
    pthread_cond_wait(p->cond, p->lock);
    pthread_mutex_unlock(p->lock);
}

void person_done(Person *p)
{
    Elevator *el = p->e;
    pthread_mutex_lock(el->lock);
    pthread_cond_signal(el->cond);
    pthread_mutex_unlock(el->lock);
}

void *elevator(void *arg)
{
    //LF
    check_for_people_to_unload();

    //LF
    check_for_people_to_load();

    //MK
    move();
    return NULL;
}
