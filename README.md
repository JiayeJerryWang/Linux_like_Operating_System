Linux-Like Operating System
Overview
This project is a custom-built Linux-like operating system developed in C for x86 architecture using QEMU as the emulator. The OS implements core functionalities such as system calls, context switching, multi-level paging, and interrupt handling. It features an Ext4 filesystem and supports multi-task execution with round-robin scheduling.

Key components include:
Interrupt Descriptor Table (IDT) and Programmable Interrupt Controller (PIC) for managing hardware interrupts.
Multi-level paging memory management for efficient memory handling.
Multi-terminal switching display for concurrent task execution.
Ext4 filesystem for file management.
System Calls: Basic system call handling for I/O and process management.
Context Switching: Supports round-robin scheduling with multi-tasking enabled through the Programmable Interval Timer (PIT).
Interrupts and Exceptions: Proper handling of interrupts, signals, and system exceptions for robust operation.
Keyboard driver for handling user input.
Real-time clock driver for time-based events.
Multi-terminal Switching: Allows users to switch between different terminal sessions seamlessly.
