#MiniAsm

A tiny asm-like language for playing around with integers

##Syntax

See examples for syntax examples

Every command takes one or two arguments.

 - To access a register, use `A-H`
 - To reference a location in RAM, use `&N` where `N` is the address. Default address space is 0 - 1023
 - To access a location in RAM who's address is stored in a register use `&A`. *Replace `A` with any register name*.
 - Lines comments start with `;`
 - Labels must be alpha characters and end with `:`. They must be on their own line.
 - To reference a label, just use the label name *(`jp` and `jnz`)*

##Command List

 - **ld** `adr1 adr2` copy the int stored in `adr2` into `adr1`
 - **ld** `reg1 reg2` copy the value stored in `reg2` into `reg1`
 - **ld** `adr lit` load the value of `lit` into `adr`
 - **ld** `reg lit` load the value of `lit` into `reg`
 - **ld** `red adr` copy the int at `adr` into `red`
 - **ld** `&reg adr` copy the int stored at `adr` into the address stored in `reg`
 - **ld** `&reg1 reg2` copy the int stored at `reg2` into the address stored in `reg1`
 - **ld** `reg1 &reg2` copy the value stored in RAM at the address stored in the location in register `reg2` to `reg1`
 - **prt** `reg` print the value stored in `reg` to the console
 - **prtch** `reg` convert the value stored in `reg` to an ASCII character and print the character to the console
 - **jp** `loc` jump to a label in the code
 - **jnz** reg loc` if the value in `reg` is not zero, jump to loc
 - **add** `reg1 reg2` add the value stored in `reg2` to the value in `reg1`
 - **sub** `reg1 reg2` subtract the value stored in `reg2` from the value in `reg1`
 - **mul** `reg1 reg2` multiply the value stored in `reg2` with the value in `reg1`
 - **div** `reg1 reg2` divide the value stored in `reg2` from the value in `reg1`
 - **rem** `reg1 reg2` divide the value stored in `reg2` to the value in `reg1` and store the remainder in `reg1`
 - **and** `reg1 reg2` bitwise and the value stored in `reg2` with the value in `reg1`
 - **or** `reg1 reg2` bitwise or the value stored in `reg2` with the value in `reg1`
 - **inc** `reg` increment the value in `reg`
 - **dec** `reg` decrement the value in `reg`
