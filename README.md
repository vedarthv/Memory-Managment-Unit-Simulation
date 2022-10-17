## Design and Implementation of a Virtual Memory Unit (MMU)


This project consists of writing a program that translates logical to physical addresses for a virtual address space of 
size 2<sup>16</sup> = 65,536 bytes. The program will read from a file containing logical addresses and, using a TLB and a page 
table, translates each logical address to its corresponding physical address and output the value of the byte stored 
at the translated physical address. The learning goal is to use simulation to understand the steps involved in translating 
logical to physical addresses. This will include resolving page faults using demand paging, managing a TLB, and 
implementing a page-replacement algorithm.

### More Specifications   
The program will read a file containing several 32-bit integer numbers that represent logical addresses. However, 
we need only be concerned with 16-bit addresses, so we must mask (i.e., ignore) the high-order 16 bits of each logical address. 
The low-order 16 bits are divided into (1) an 8-bit page number and (2) an 8-bit page offset. Hence, the addresses are 
structured as shown as:  

<!-- ![Virtual Address](./figs/address.png) -->
<img src="./figs/address.png" alt="alt text" width="500">

Other specifics include the following:
* 2<sup>8</sup> entries in the page table 
* Page size of 2<sup>8</sup> bytes 
* 16 entries in the TLB 
* Frame size of 2<sup>8</sup> bytes 
* 256 frames 
* Physical memory of 65,536 bytes (256 frames × 256-byte frame size)    


Additionally, the program only needs to be concerned with reading logical
addresses and translating them to their corresponding physical addresses. We do not need to support writing to the 
logical address space. 
