#![feature(mutex_unlock)]

use std::os::raw::c_char;
use std::ffi::CString;
use lazy_static::lazy_static;
use std::sync::{Arc, Mutex, Condvar};
use std::sync::atomic::AtomicI32;

mod constants;
use constants::*;

struct Elevator {
    at: i32,
    direction: i32,
    wait_at: i32,
    is_elevator_ready: bool,
    is_passenger_ready: bool,
}

extern "C" {
    fn log_msg(level: i32, msg: *const c_char);
}

lazy_static!{
    static ref ONE_AT_A_TIME: Mutex<()> = Mutex::new(());
    static ref ELEVATOR: (Mutex<Elevator>, Condvar, Condvar) = 
    (
        Mutex::new(Elevator{at: 0, direction: 1, wait_at: -1, is_elevator_ready: false, is_passenger_ready: false}),
        Condvar::new(), // condition variable for elevator
        Condvar::new() // condition variable for passenger
    );
}

pub fn log(msg: String) {
    unsafe {
        let cs = CString::new(msg).unwrap();
        log_msg(9,cs.as_ptr());
    }
}

/********************************************************************************************************************
TODO 1a: This is not needed for the Rust Lab, only kept here to be in sync with the lab document
********************************************************************************************************************/

#[no_mangle]
pub fn scheduler_init() {
//    println!("scheduler init!");
}

#[no_mangle]
pub fn passenger_request(passenger: i32, from_floor: i32, to_floor: i32, 
                         enter: fn(i32, i32), exit: fn(i32, i32)) {
    // grab lock to ensure one passenger at a time
    let _ticket = ONE_AT_A_TIME.lock();

    // **************
    // ENTER THE LIFT
    // **************

    let elevator = &ELEVATOR.0;
    // grab lock for guarding elevator data
    let mut el = elevator.lock().unwrap();

    // TODO 1b: Submit request to elevator by setting 'wait_at'.
    // elevator struct's 'wait_at' is used to store the next floor at which the elevator should
    // stop at. Replace 0 below with the correct value for this request.
    el.wait_at = 0;

    // TODO 1c:
    // After setting the 'wait_at' variable, we wait for the elevator to arrive at the
    // floor and inform us once it's ready.
    //
    // The passenger thread should wait for the 'is_elevator_ready' variable to be set
    // An elevator will set this variable and signal this thread
    //
    // Fill in the while loop below to wait for the condition to be met.
    while false /* FILL IN CORRECT CONDITION HERE */ {
        // el = <make the condition variable for the elevator to wait>
    }
    el.is_elevator_ready = false;

    // enter the lift
    enter(passenger, 0);

    // TODO 1e:
    // We've now entered the elevator. It's now the passenger's turn to notify the elevator
    // that they have entered. As in the previous TODOs, use notify_one on the condition 
    // variable for the passenger to notify the elevator thread that the passenger is ready.
    /* FILL IN HERE */
    // el.is_passenger_ready = ?;
    // (?).notify_one();

    // *************
    // EXIT THE LIFT
    // *************
    // TODO 2: Use what you've learned in the previous TODOs to replace the below busy poll
    // with the condition algorithm.
    loop {
        if el.at == to_floor {        
            exit(passenger, 0);
            break;
        }
    }
}

#[no_mangle]
pub fn elevator_ready(elevator_no: i32, at_floor: i32, 
                      move_direction: fn(i32, i32), 
                      door_open: fn(i32), door_close: fn(i32)) {

    let elevator = &ELEVATOR.0;
    // grab lock for guarding elevator data
    let mut el = elevator.lock().unwrap();
    
    if elevator_no == 0 {
        match at_floor {
            floor if floor == FLOORS-1 => {el.direction = -1;}
            0 => {el.direction = 1;}
            _ => {}
        }

        if el.wait_at != -1 {
            if el.wait_at == at_floor {
                door_open(elevator_no);

                // There is a passenger waiting at this floor.
                // We set wait_at back to its default value so it can be used later.                
                el.wait_at = -1;

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

                while false /* FILL IN CORRECT CONDITION HERE */ {
                    /* WAIT FOR CONDITION HERE */
                }
                
                el.is_passenger_ready = false;
                door_close(elevator_no);
            }
        }
        else {
            // There's no passenger waiting at this floor
            // Let's release the lock which will allow other passenger threads to run
            // and check the state of the elevator
            Mutex::unlock(el);
            std::thread::sleep(std::time::Duration::from_micros(1));
            el = (&ELEVATOR.0).lock().unwrap();
        }

        move_direction(elevator_no,el.direction);
        el.at += el.direction;
    }
}
