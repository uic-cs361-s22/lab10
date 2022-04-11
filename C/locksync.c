#include"elevator.h"
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>

// #define USECONDITION // TODO 1a: uncomment this line

pthread_mutex_t passenger_lock;
pthread_mutex_t elevator_lock;
int elevator_floor=0;
int elevator_direction=1;

int wait_at = -1;
int is_elevator_ready = 0;
int is_passenger_ready = 0;
pthread_cond_t elevator_signal = PTHREAD_COND_INITIALIZER;
pthread_cond_t passenger_signal = PTHREAD_COND_INITIALIZER;

void scheduler_init() {
    pthread_mutex_init(&passenger_lock,0);
    pthread_mutex_init(&elevator_lock,0);
    pthread_mutex_lock(&elevator_lock);
}

#ifndef USECONDITION
/********************************************************************************************************************
Method1: Implementation using busy poll
********************************************************************************************************************/
void passenger_request(int passenger, int from_floor, int to_floor, void (*enter)(int, int), void(*exit)(int, int)) {
    pthread_mutex_lock(&passenger_lock);

    while(1) {
        pthread_mutex_lock(&elevator_lock);
        if(elevator_floor == from_floor) { 
            enter(passenger,0);
            pthread_mutex_unlock(&elevator_lock);
            break;
        }
        pthread_mutex_unlock(&elevator_lock);
    }

    while(1) {
        pthread_mutex_lock(&elevator_lock);
        if(elevator_floor == to_floor) { 
            exit(passenger,0);
            pthread_mutex_unlock(&elevator_lock);
            break;
        }
        pthread_mutex_unlock(&elevator_lock);
    }

    pthread_mutex_unlock(&passenger_lock);
}

void elevator_ready(int elevator, int at_floor, void(*move_direction)(int, int), void(*door_open)(int), void(*door_close)(int)) {
    if(elevator == 0) {
        if(at_floor == FLOORS-1)
            elevator_direction = -1;
        if(at_floor == 0)  
            elevator_direction = 1;

        door_open(elevator);

        pthread_mutex_unlock(&elevator_lock);
        usleep(1);
        pthread_mutex_lock(&elevator_lock);

        door_close(elevator);      
          
        move_direction(elevator,elevator_direction);
        elevator_floor = at_floor+elevator_direction;
    }
}

#else

/********************************************************************************************************************
Method2: Implementation using conditions

In this method we replace the busy polling with conditions.
TODO 1a: define USECONDITION to begin using this method. 
********************************************************************************************************************/
void passenger_request(int passenger, int from_floor, int to_floor, void (*enter)(int, int), void(*exit)(int, int)) {
    pthread_mutex_lock(&passenger_lock);

    // *************
    // ENTER THE LIFT
    // *************
    pthread_mutex_lock(&elevator_lock);

    // TODO 1b: Submit request to elevator by setting 'wait_at'.
    // 'wait_at' is used to store the next floor at which the elevator should stop
    // at. Replace 0 below with the correct value for this request.
    wait_at = 0;

    // TODO 1c:
    // After setting the 'wait_at' variable, we wait for the elevator to arrive at the
    // floor and inform us once it's ready.
    //
    // The passenger thread should wait for the 'is_elevator_ready' variable to be set
    // and the elevator sets the variable and signals this thread
    //
    // Fill in the while loop below to wait for the condition to be met.
    while(0 /* FILL IN CORRECT CONDITION HERE */) {
        // pthread_cond_wait(_, _);
    }
    is_elevator_ready = 0;

    // enter the lift
    enter(passenger, 0);

    // TODO 1e:
    // We've now entered the elevator. It's now the passenger's turn to notify the elevator
    // that they have entered. As in the previous TODOs, use pthread_cond_signal to notify
    // the elevator thread that the passenger is ready.
    /* FILL IN HERE */
    pthread_mutex_unlock(&elevator_lock);


    // *************
    // EXIT THE LIFT
    // *************
    // TODO 2: Use what you've learned in the previous TODOs to replace the below busy poll
    // with the condition algorithm.
    while(1) {
        pthread_mutex_lock(&elevator_lock);
        if(elevator_floor == to_floor) { 
            exit(passenger,0);
            pthread_mutex_unlock(&elevator_lock);
            break;
        }
        pthread_mutex_unlock(&elevator_lock);
    }

    pthread_mutex_unlock(&passenger_lock);
}

void elevator_ready(int elevator, int at_floor, void(*move_direction)(int, int), void(*door_open)(int), void(*door_close)(int)) {
    if(elevator == 0) {
        if(at_floor == FLOORS-1)
            elevator_direction = -1;
        if(at_floor == 0)  
            elevator_direction = 1;

        door_open(elevator);

        if (wait_at == at_floor) {
            // There is a passenger waiting at this floor.
            // We set wait_at back to its default value so it can be used later.
            wait_at = -1;

            // TODO 1d:
            // The passenger thread waiting for the elevator will be waiting for the elevator
            // to be ready so that it can step into the elevator. The door is open now so the elevator
            // is ready! Time to signal the other thread.
            //
            // Signal the passenger thread using 'is_elevator_ready', 'elevator_signal' and pthread_cond_signal
            /* FILL IN CODE HERE */

            // TODO 1f:
            // It's now the passengers turn to notify the elevator thread once it's entered.
            // Wait for the 'is_passenger_ready' condition to be ready. Use the 'passenger_signal'
            // variable along with pthread_cond_wait to wait.
            while(0 /* FILL IN CORRECT CONDITION HERE */) {
               /* WAIT FOR CONDITION HERE */
            }
            is_passenger_ready = 0;
        } else {
            // There's no passenger waiting at this floor
            // Let's release the lock which will allow other passenger threads to run
            // and check the state of the elevator
            pthread_mutex_unlock(&elevator_lock);
            usleep(1);
            pthread_mutex_lock(&elevator_lock);
        }

        door_close(elevator);
          
        move_direction(elevator,elevator_direction);
        elevator_floor = at_floor+elevator_direction;
    }
}
#endif