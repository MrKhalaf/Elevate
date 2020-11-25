extern void move_to_floor(Elevator *e, int floor);     /* Blocks until the elevator is at the given floor. */

extern void open_door(Elevator *e);                    /* Blocks until the elevator's door is open. */

extern void close_door(Elevator *e);                   /* Blocks until the elevator's door is closed. */

extern void get_on_elevator(Person *p);               /* Called when a person gets on an elevator.  The elevator
                                                       must be on the person's "from" floor with the door open.
                                                       This will add the person to the elevator's list (setting
                                                       the person's ptr appropriately. */

extern void get_off_elevator(Person *p);              /* Called when a person gets off an elevator.  The elevator
                                                       must be on the person's "to" floor with the door open,
                                                       and the person must be on the elevator's dllist.  The person's
                                                       ptr will be used to delete the person from the elevator's dllist. */

extern void *person(void *arg);                  /* The person thread */

/* ------------------------------------------------------------------ */
/* Procedures that you must define: */

extern void initialize_simulation(Elevator_Simulation *es);  /* Called once at the beginning of the simulation */
extern void initialize_elevator(Elevator *e);                /* Called once for each elevator, before the thread is created */
extern void initialize_person(Person *p);                    /* Called once for each person, before the thread is created */

/* The following is called when a person first enters the system.
   It should block until an elevator is on the person's "from" floor
   and the door is open.  When it does so, the person's e field
   should point to the elevator on which the person should get.  */

extern void wait_for_elevator(Person *p);


/* The following is called after a person gets on the elevator.  It should block
   until the person is at his/her destination floor, and the elevator's
   door is open. */

extern void wait_to_get_off_elevator(Person *p);


/* The following is called after a person gets off the elevator. */

extern void person_done(Person *p);


/* The following is the main elevator thread.  It can only move and open/close
   its doors using the given procedures. */

extern void *elevator(void *arg);   /* The arg is the thread's Elevator struct */
