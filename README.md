# PDP-8 Computer Simulator
This is a project which was assigned to me at the University of Montenegro. Following code simulates the behavior of a PDP-8 computer by fetching, decoding, and executing instructions stored in memory, mimicking the basic functionality of a real computer architecture.

<p align="center">
<img src="https://github.com/alfaromeo13/pdp8/assets/60315689/363b2c59-ab04-41b5-9c8d-168252cc7ccf" alt="drawing"/>
</p>

  ### Absolute Loader:
  Reads a program file (Program.txt) and loads it into the computer's memory (RAM). The file contains assembly-like instructions.

  ### Fetch Cycle:
  This cycle fetches instructions from memory. It involves setting up the Memory Address Register (MAR) with the Program Counter (PC) value and loading the Memory Buffer Register (MBR) with the instruction from the specified memory address.

  ### Indirect Cycle:
  If the instruction requires indirection, this cycle is activated. It involves fetching the operand address from memory based on the address stored in the MBR.

  ### Execute Cycle:
  Executes the fetched instruction. This involves various operations depending on the instruction type:
 
  - For memory instructions (e.g., AND, ADD, LDA), it performs operations between the Accumulator (AC) and memory data.
  - For register instructions (e.g., CLA, CLE), it performs operations on the accumulator and other control registers.
  - Updates the Program Counter (PC) for branching instructions (e.g., BUN, BSA).
  - Handles special instructions like HALT (HLT).

  ### Clock Cycle Generation: 
  Controls the flow of execution by generating clock cycles. Each clock cycle corresponds to a specific phase of the instruction cycle (Fetch, Indirect, Execute).

  ### Binary to Decimal Conversion:
  Utility functions convert binary numbers to decimal for various operations.

### Original documentation:
  http://www.bitsavers.org/pdf/dec/pdp8/pdp8/F-81_PDP-8_Brochure_Mar65.pdf
