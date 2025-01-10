💻 Virtual Memory Page Table Simulation

This project simulates the concept of a virtual memory page table. It parses a source file, calculates the sizes of various memory sections (text, rodata, data, bss), initializes page table entries (PTEs), and simulates address translation between virtual and physical memory. The program also handles segmentation faults and page faults.

📎 Prerequisites

1. C Compiler (e.g., GCC)
2. Linux/Unix environment (for compilation and execution)
3. Basic knowledge of memory management concepts (virtual memory, page tables)

📎 Installation

1. Clone the repository:
    git clone https://github.com/your-username/virtual-memory-simulation.git
2. Navigate to the project directory:
    cd virtual-memory-simulation
3. Compile the program:
    gcc -o memory_simulation memory_simulation.c functions.c

📎 Usage

1. Running the Program
    To run the program, you need to pass the path to a source file as a command-line argument. The source file should contain information about the sections of the program (text, rodata, data, bss).
2. Example command:
./memory_simulation source_file.c
3. Example Output
Once the program is running, you will be prompted to enter a virtual address for access. For example:

    Enter virtual address to access. start-1000: Enter 'q' for exit.
    1000
    Physical address: 0x4000

    If an invalid address is entered (outside the valid range of sections), the program will output:
    Segmentation fault.

    If a page fault occurs (i.e., a page hasn't been loaded into memory yet), it will output:
    Page fault.


📎  File Structure

/virtual-memory-simulation
├── memory_simulation.c      # Main program implementation
├── myLib.h                  # Header file with function declarations
├── README.md                # Project documentation
└── source_file.c            # Sample source file (if any)


