# SMART-PARKING-SYSTEM
Smart Parking Record System — a C++ DSA mini project that automates vehicle entry/exit, slot allotment, billing, and a waiting queue using linked lists, stacks, and queues (no STL containers).
# 🅿️ Smart Parking Record System

A console-based parking management system built in C++ as a Data Structures
mini project. It automatically handles vehicle entry, exit, slot allotment,
fee calculation, and a waiting queue — no manual computation needed.

## Features
- Automatic slot allotment and release
- Auto-generated bill on exit (based on vehicle type and duration)
- Waiting queue when the lot is full, with automatic slot handoff
- Exit history with running total collection
- Search for any vehicle (parked or waiting)

## Data Structures Used
| Structure            | Purpose                                   |
|-----------------------|--------------------------------------------|
| Linked List            | Tracks currently parked vehicles           |
| Stack (array-based)    | Manages free parking slots                 |
| Queue (linked)          | Holds vehicles waiting for a free slot     |
| Stack (linked)          | Stores exit history (latest on top)        |

## How to Run
\`\`\`bash
g++ -std=c++17 smart_parking.cpp -o parking
./parking
\`\`\`

## Tech
C++17, no external libraries.
