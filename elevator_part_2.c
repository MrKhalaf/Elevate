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
pthread_mutex_t *global_lock;

void initialize_simulation(Elevator_Simulation *es)
{
    /* Called once at the beginning of the simulation */
    people_list = new_dllist();
    holding = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(holding, NULL);

    arrived = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(arrived, NULL);

    global_lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(global_lock, NULL);

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

void check_for_people_to_unload(Elevator * el, Dllist to_unload) {
    Dllist temp = el->people->flink;
    //iterate through people on the elevator
    while (temp != to_unload && to_unload != NULL) {
        Person *p = (Person *) (temp->val.v);

        //if person is getting off, signal person
        if (el->onfloor == p->to) {
            if (el->door_open == 0) {
                open_door(el);
            }

            pthread_mutex_lock(p->lock);
            pthread_cond_signal(p->cond);
            pthread_mutex_lock(el->lock);
            pthread_cond_wait(el->cond, el->lock);
            pthread_mutex_unlock(el->lock);
        }
        temp = temp->flink;

    }
}

void check_for_people_to_load(Elevator * el, Dllist to_unload, int direction) {
    pthread_mutex_lock(global_lock);
    Dllist temp = people_list->flink;
    //while (!dll_empty(temp) || temp != NULL) {
    while (temp != people_list && people_list != NULL) {
        Person * p = (Person *)(temp->val.v);

        if (  (el->onfloor == p->from) && (((p->to > el->onfloor) && (direction == 1)) || ((p->to < el->onfloor) && (direction == 0)))) {
            dll_append(to_unload, temp->val);
        }

        temp = temp->flink;
    }

    pthread_mutex_unlock(global_lock);
}

void *elevator(void *arg)
{

    Elevator *el = (Elevator *) arg;
    Person *p;
    int direction = 1;
    Dllist to_unload;

    while (1) {

        //if no people in simulation yet
        pthread_mutex_lock(el->es->lock);
        while(people_list == NULL) {
            pthread_cond_wait(holding, el->es->lock);
        }
        pthread_mutex_unlock(el->es->lock);

        if (el->onfloor == 1) {
            direction = 1;
        } else if (el->onfloor == el->es->nfloors) {
            direction = 0;
        }

        //LF
        if (!dll_empty(people_list)) {
            check_for_people_to_load(el, to_unload, direction);

            Dllist temp = to_unload->flink;
            while (temp->flink != NULL) {
                p = (Person *) temp->val.v;
                if (el->door_open == 0) {
                    open_door(el);
                }

                pthread_mutex_lock(p->lock);
                p->e = el;
                pthread_cond_signal(p->cond);
                pthread_mutex_lock(el->lock);
                pthread_mutex_unlock(p->lock);

                pthread_cond_wait(el->cond, el->lock);
                pthread_mutex_unlock(el->lock);
                temp = temp->flink;
            }
        }

        if (el->door_open == 1) {
            close_door(el);
        }

        if (direction == 1) {
            move_to_floor(el, el->onfloor+1);
        } else if (direction == 0) {
            move_to_floor(el, el->onfloor-1);
        }

        //LF
        check_for_people_to_unload(el, to_unload);


    }

    return NULL;
}
