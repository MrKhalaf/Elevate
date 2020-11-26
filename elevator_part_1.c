/* elevator_null.c
   Null solution for the elevator threads lab.
   Jim Plank
   CS560
   Lab 2
   January, 2009
 */

#include <stdio.h>
#include <pthread.h>
#include "elevator.h"

Dllist people_list;
pthread_cond_t arrived;
pthread_cond_t holding;

void initialize_simulation(Elevator_Simulation *es)
{
    /* Called once at the beginning of the simulation */
    people_list = new_dllist();
    holding = PTHREAD_COND_INITIALIZER;

}

void initialize_elevator(Elevator *e)
{
    /* Called once for each elevator, before the thread is created */
    e->onfloor = 0;
    e->door_open = 0;
    e->moving = 0;
    e->people = NULL;
    e->cond = holding;
    e->lock = PTHREAD_MUTEX_INITIALIZER;

}

void initialize_person(Person *e)
{
    /* Called once for each person, before the thread is created */
    e->from = 0;
    e->cond = arrived;
    e->lock = PTHREAD_MUTEX_INITIALIZER;

}

void wait_for_elevator(Person *p)
{
    /* The following is called when a person first enters the system.
   It should block until an elevator is on the person's "from" floor
   and the door is open.  When it does so, the person's e field
   should point to the elevator on which the person should get.

     : append the person to the global list. Signal the
condition variable for blocking elevators. Block on the person’s condition variable

     */
    dll_append(people_list, Jval val)
}

void wait_to_get_off_elevator(Person *p)
{
    /* The following is called after a person gets on the elevator.  It should block
   until the person is at his/her destination floor, and the elevator's
   door is open. */
}

void person_done(Person *p)
{
    /* The following is called after a person gets off the elevator. */
}

void *elevator(void *arg)
{
    /* The following is the main elevator thread.  It can only move and open/close
   its doors using the given procedures. */
    return NULL;
}
