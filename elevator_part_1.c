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

//LF
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

//LF
void wait_to_get_off_elevator(Person *p)
{
    /* The following is called after a person gets on the elevator.  It should block
   until the person is at his/her destination floor, and the elevator's
   door is open. */

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


//MK
void person_done(Person *p)
{
    /* The following is called after a person gets off the elevator. */
    //signal elevator
    Elevator *el = p->e;
    pthread_mutex_lock(el->lock);
    pthread_cond_signal(el->cond);
    pthread_mutex_unlock(el->lock);
}


//MK
void *elevator(void *arg)
{
    /* The following is the main elevator thread.  It can only move and open/close
   its doors using the given procedures.
     */

    Elevator *el = (Elevator *) arg;
    Person *p;

    while (1) {
        // If global list is empty, block on elevators condition
        pthread_mutex_lock(el->es->lock);
        while(dll_empty(people_list)) {
            pthread_cond_wait(holding, el->es->lock);
        }
        pthread_mutex_unlock(el->es->lock);

        // When the elevator gets a person to service . . .
        pthread_mutex_lock(el->es->lock);
        p = jval_v(dll_val(dll_first(people_list)));
        dll_delete_node(people_list->flink); // may also need to delete the backlink?
        pthread_mutex_unlock(el->es->lock);

        // it moves to the appropriate floor and opens its door.
        move_to_floor(el, p->from);
        open_door(el);
        //It puts itself into the person’s e field
        p->e = el;

        // then signals the person and blocks until the person wakes it up.
        pthread_mutex_lock(el->lock);
        pthread_cond_signal(p->cond);
        pthread_cond_wait(el->cond, el->lock);
        pthread_mutex_unlock(el->lock);

        // el goes to the person’s destination floor
        close_door(el);
        move_to_floor(el, p->to);

        // opens its door, signals the person and blocks.
        open_door(el);
        pthread_mutex_lock(el->lock);
        pthread_cond_signal(p->cond);
        pthread_cond_wait(el->cond, el->lock);
        pthread_mutex_unlock(el->lock);

        // When the person wakes it up, it closes its door and re-executes its while loop.
        close_door(el);

    }
}
