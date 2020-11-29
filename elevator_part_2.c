/*
 * We certify that this code was our own work. It was written without consulting tutors or code written by other students
 * Lydia Feng and Mohammad Khalaf
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "elevator.h"

typedef struct {
    Dllist people_list;
    pthread_cond_t *holding;
    pthread_cond_t *arrived;
    pthread_mutex_t *global_lock;
}vpointer;


void initialize_simulation(Elevator_Simulation *es)
{
    /* Called once at the beginning of the simulation */
    vpointer *vpointer1 = (vpointer *)malloc(sizeof(vpointer));
    vpointer1->people_list = new_dllist();
    vpointer1->holding = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(vpointer1->holding, NULL);

    vpointer1->arrived = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(vpointer1->arrived, NULL);

    vpointer1->global_lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(vpointer1->global_lock, NULL);

    es->v = vpointer1;    //pointer to list of people, conditions, locks

}

void initialize_elevator(Elevator *e)
{
}

void initialize_person(Person *e)
{
}

void wait_for_elevator(Person *p)
{
    vpointer *position;
    position = (vpointer *)(p->es->v);

    pthread_mutex_lock(p->es->lock);
    dll_append(position->people_list, new_jval_v(p));
        pthread_cond_signal(position->holding);
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

void check_for_people_to_unload(Elevator * el, Dllist to_unload, vpointer * el_vpointer) {

    Dllist temp = dll_first(el->people);
    while (temp != dll_nil(el->people) && !dll_empty(el->people)) {
        Dllist next = dll_next(temp);
        Person *p = (Person *) temp->val.v;
        if (p->to == el->onfloor) {
            if (el->door_open == 0) {
                open_door(el);
            }
            pthread_mutex_lock(p->lock);
            pthread_cond_signal(p->cond);
            pthread_mutex_lock(el->lock);
            pthread_mutex_unlock(p->lock);
            pthread_cond_wait(el->cond, el->lock);
            pthread_mutex_unlock(el->lock);
        }
        temp = next;

    }
}

void check_for_people_to_load(Elevator * el, Dllist to_unload, int direction, vpointer * el_vpointer) {

    pthread_mutex_lock(el_vpointer->global_lock);

    Dllist temp;
    temp = dll_first(el_vpointer->people_list);

    while (temp != el_vpointer->people_list && !dll_empty(el_vpointer->people_list)) {

        Person * p = (Person *)(temp->val.v);
        Dllist temp2 = dll_next(temp);

        if (  (el->onfloor == p->from) && (((p->to > el->onfloor) && (direction == 1)) || ((p->to < el->onfloor) && (direction == 0)))) {
            dll_append(to_unload, temp->val);

            dll_delete_node(temp);
        }

        temp = temp2;
    }

    pthread_mutex_unlock(el_vpointer->global_lock);
}

void *elevator(void *arg)
{

    Elevator *el = (Elevator *) arg;
    Person *p;
    int direction = 1;
    Dllist to_unload = new_dllist();
    vpointer *el_vpointer = (vpointer *)(el->es->v);

    while (1) {

        if (el->onfloor == 1) {
            direction = 1;
        } else if (el->onfloor == el->es->nfloors) {
            direction = 0;
        }
        //LF

        check_for_people_to_load(el, to_unload, direction, el_vpointer);

        Dllist temp = to_unload->flink;
        while (temp != to_unload && !dll_empty(to_unload)) {
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

            Dllist temp2 = temp->flink;
            dll_delete_node(temp);
            temp = temp2;

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
        check_for_people_to_unload(el, to_unload, el_vpointer);


    }

    return NULL;
}
