# simbly

Multithreaded runtime for a simple dynamically typed assembly-like language. All variables are of type integer or array of integers. Has a small shell and supports running multiple programs simultaneously.

In the pic below you can see, on the left column, the grammar of the language, and on the right column a pseudo-C meaning of what each instruction does.

![](https://i.imgur.com/KgMjhTy.png) 

All programs have their own local variables that they can operate on, sort of like registers. There are also global variables that all programs can read and write to, sort of like a RAM.

All variables are defined when they are first used. If no value is assigned to them they will be initialized with the value 0. In this program

```
#PROGRAM
PRINT "val =" $val
```

`val` wasn't initialized to anything, therefore the interpreter will initialize a local variable with the name `val` and assign to it the value 0. That program will simply print `val = 0`.

Globals can be seen from all the programs and are dynamically typed just like locals. However, globals can only be accessed through the instructions `LOAD` and `STORE` and no immediate operations would work on them. They can also be used like a semaphore, to synchronize with other running programs, through the instructions `DOWN` and `UP`.

This program

```
#PROGRAM
STORE $g 15
LOAD $x $g
PRINT "x g =" $x $g
```
creates a global named `g` and initializes it to 15. It assigns the value of `g` to a local variable `x` and then prints `x` and `g`. The `PRINT` instruction can only print local variables and it will try to find a local named `g`, but since there are no locals with that name, it will instead create one. By the end the program will have two locals named `x` and `g`, and one global named `g`.

It will print `x g = 15 0`.

Sample programs that implement various concurrency problems can be found in the `test_programs` folder. Rend1 and Rend2 are for the rendezvous problem. Init, Producer and Consumer are for the producer/consumer problem (you need to execute Init.txt first, to initialize the global variables). Barber and Customer are for the sleeping barber problem. InitRW, Reader and Writer are for the reader/writer problem (you need to execute InitRW.txt first).

## Building

Supported OS is Linux and compiler is gcc. Might work on BSD too but it hasn't been tested there.

External dependency is [libvoids](https://github.com/Gikoskos/libvoids) which is used for all the data structures in the program. Clone this repository with the `recursive` flag:

`git clone --recursive https://github.com/Gikoskos/simbly`

to download all the submodules.

Build system is cmake:

`mkdir build && cd build && cmake .. && make`

## How to use

Run the command `help`, after executing the program, to get a list of all the possible commands.

## License

see LICENSE

This was a lab project for UTH
