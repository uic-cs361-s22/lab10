FLOORS?=28
ELEVATORS?=4
CAPACITY?=3
PASSENGERS?=50
CFLAGS=-lpthread -lncurses -ldl
FILES=elevators
LOG_LEVEL=9

elevators: elevator/src/lib.rs ../C/main.c
	FLOORS=$(FLOORS) ELEVATORS=$(ELEVATORS) PASSENGERS=$(PASSENGERS) CAPACITY=$(CAPACITY) envsubst < elevator/src/constants.tmpl > elevator/src/constants.rs
	cd elevator; cargo build; cd ..
	gcc -DFLOORS=$(FLOORS) -DELEVATORS=$(ELEVATORS) -DLOG_LEVEL=$(LOG_LEVEL) -DCAPACITY=$(CAPACITY) -DPASSENGERS=$(PASSENGERS) -I../C -Lelevator/target/debug ../C/main.c -o $@ -lelevator $(CFLAGS)

clean:
	rm -f $(FILES) *.o 
	cd elevator ; cargo clean ; rm src/constants.rs; cd ..
