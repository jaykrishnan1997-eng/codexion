*This project has been created as part of the 42 curriculum by jkrishna.*

# Codexion

## Description

Codexion is a concurrency and synchronization project written in C as part of the 42 curriculum.

The project simulates a group of coders working around a shared quantum compiler. Each coder is represented by a POSIX thread and must repeatedly go through the following cycle:

```text
        ┌─────────────┐
        │   Compile   │
        │ 2 dongles   │
        └──────┬──────┘
               │
               ▼
        ┌─────────────┐
        │   Debug     │
        └──────┬──────┘
               │
               ▼
        ┌─────────────┐
        │  Refactor   │
        └──────┬──────┘
               │
               ▼
        Try to acquire the
        two dongles again
```

The difficulty is that the dongles are shared resources. A coder can compile only while holding both required dongles. Multiple coders may request the same dongle at the same time, so access must be synchronized and granted according to the selected scheduling policy.

The program supports two arbitration policies:

- **FIFO** — First In, First Out. Requests are served according to their arrival sequence.
- **EDF** — Earliest Deadline First. Requests are ordered according to the coder's burnout deadline:

```text
deadline = last_compile_start + time_to_burnout
```

The project therefore combines:

- POSIX threads with `pthread_create()` and `pthread_join()`
- mutexes
- condition variables
- priority queues implemented as binary heaps
- FIFO and EDF scheduling
- shared-resource management
- burnout monitoring
- precise millisecond timing
- synchronized logging
- dynamic memory management and cleanup

There are no global variables.

---

## Program Requirements

The executable is named:

```text
codexion
```

It takes exactly eight mandatory arguments:

```text
./codexion number_of_coders time_to_burnout time_to_compile \
          time_to_debug time_to_refactor number_of_compiles_required \
          dongle_cooldown scheduler
```

### Arguments

| Argument | Description |
|---|---|
| `number_of_coders` | Number of coder threads and, for multiple coders, number of dongles |
| `time_to_burnout` | Maximum time a coder may go without starting another compilation |
| `time_to_compile` | Time spent compiling while holding both dongles |
| `time_to_debug` | Time spent debugging after compilation |
| `time_to_refactor` | Time spent refactoring after debugging |
| `number_of_compiles_required` | Number of successful compilations required from every coder before normal termination |
| `dongle_cooldown` | Time a released dongle remains unavailable |
| `scheduler` | `fifo` or `edf` |

All times are expressed in milliseconds.

Invalid arguments such as negative numbers, non-integers, missing arguments, extra arguments, or an invalid scheduler must be rejected.

---

## Instructions

### Compilation

The project can be compiled with:

```bash
make
```

The Makefile uses:

```text
cc
-Wall
-Wextra
-Werror
-pthread
```

Other available Makefile targets are:

```bash
make all
make clean
make fclean
make re
```

### Execution

Example:

```bash
./codexion 3 2000 200 100 100 3 50 fifo
```

EDF example:

```bash
./codexion 3 2000 200 100 100 3 50 edf
```

The scheduler must be exactly:

```text
fifo
```

or:

```text
edf
```

---

## Simulation Rules

### Coders

Every coder is represented by a `pthread_t`.

Coder IDs start at `1` and continue to `number_of_coders`.

The coders are arranged in a circle:

```text
1 -- 2 -- 3 -- ... -- N
|                   |
+-------------------+
```

Coder `1` is next to coder `N`.

Each coder has a left and right dongle. With several coders, each dongle is shared by the two adjacent coders.

The single-coder case is special: when there is only one coder, there is only **one dongle**, and the coder uses that same dongle for both sides.

### Compilation

A coder must acquire both required dongles before it can start compiling.

The log therefore contains two:

```text
has taken a dongle
```

messages immediately before:

```text
is compiling
```

After compilation, both dongles are released and enter their cooldown period.

### Debugging and refactoring

After compiling, the coder:

1. releases both dongles;
2. debugs;
3. refactors;
4. attempts to acquire the required dongles again.

---

## Scheduling

### FIFO

With FIFO scheduling, a dongle serves the request that arrived first.

Each request receives a monotonically increasing sequence number. The sequence number is protected by a mutex because multiple coder threads may create requests concurrently.

The request heap is ordered by this sequence number when the scheduler is `fifo`.

### EDF

With EDF scheduling, the request with the earliest burnout deadline has priority.

The deadline is:

```text
last_compile_start + time_to_burnout
```

The request heap is ordered by this deadline when the scheduler is `edf`.

The heap implementation is shared by both scheduling modes; only the comparison rule changes.

---

## Priority Queue / Heap

The project implements its own binary min-heap because a standard C priority queue is not available.

Each heap node contains:

```text
coder
sequence
deadline
```

The heap supports:

- insertion
- extraction of the minimum-priority request

For FIFO, the sequence number determines priority.

For EDF, the burnout deadline determines priority.

Each dongle owns its own request heap. The dongle mutex protects access to the dongle state and its request heap.

---

## Dongle Cooldown

A released dongle cannot immediately be reused.

When a coder releases a dongle:

```text
available_at = current_time + dongle_cooldown
```

The dongle remains unavailable until that timestamp has been reached.

A request therefore has to satisfy all of the relevant conditions before it can acquire a dongle:

- the dongle is not currently in use;
- its cooldown has expired;
- the requesting coder is the highest-priority request according to FIFO or EDF.

A condition variable is used to wake waiting coders when the dongle state changes.

---

# Blocking Cases Handled

Concurrency is the central difficulty of Codexion. The implementation is designed around the following blocking cases.

## Deadlock

A classic deadlock risk exists if coders acquire one dongle and then wait indefinitely for the second one.

For example:

```text
Coder 1 holds dongle A
Coder 2 holds dongle B

Coder 1 waits for B
Coder 2 waits for A
```

This creates a circular wait.

The implementation prevents this particular acquisition-order problem by imposing an ordering on the two dongles:

```text
acquire lower dongle ID first
acquire higher dongle ID second
```

The single-coder case is handled separately because both logical sides refer to the same physical dongle.

This approach is based on breaking Coffman's **circular-wait** condition.

The four classic Coffman conditions are:

1. Mutual exclusion
2. Hold and wait
3. No preemption
4. Circular wait

Mutual exclusion is unavoidable because a dongle can only be used by one coder at a time. The acquisition ordering is used to prevent a circular wait from forming.

## Resource duplication

Every dongle has its own:

```c
pthread_mutex_t mutex;
```

The mutex protects its shared state, including:

- whether it is currently in use;
- its cooldown timestamp;
- its request heap.

A coder cannot simultaneously be granted the same dongle as another coder.

## Starvation

FIFO provides arrival-order arbitration.

EDF prioritizes the coder whose burnout deadline is closest:

```text
last_compile_start + time_to_burnout
```

The scheduler therefore does not simply favour the coder that happened to arrive first in EDF mode.

The project also stresses EDF under contention to check that coders do not remain indefinitely waiting while feasible progress is possible.

## Cooldown blocking

A dongle that has just been released is deliberately kept unavailable until:

```text
available_at
```

is reached.

A waiting coder cannot bypass this condition merely because it has the highest-priority request.

## Logging races

Multiple coder threads can attempt to print at the same time.

All state-change output is therefore protected by:

```c
pthread_mutex_t log_mutex;
```

This guarantees that two log messages cannot be mixed together on the same output line.

The expected format is:

```text
timestamp coder_id message
```

For example:

```text
0 1 has taken a dongle
1 1 has taken a dongle
1 1 is compiling
201 1 is debugging
401 1 is refactoring
```

## Shared simulation state

The simulation termination flag is shared between the coder threads and the monitor thread.

It is protected by:

```c
pthread_mutex_t sim_mutex;
```

The sequence counter used when creating scheduling requests is protected by:

```c
pthread_mutex_t sequence_mutex;
```

Each coder also has a state mutex protecting coder-specific shared state such as its compilation count and compilation timestamp.

---

# Burnout Detection

Burnout is one of the most important parts of the project.

A coder burns out when it has not started a new compilation within:

```text
time_to_burnout
```

milliseconds from either:

- the beginning of the simulation for the first compilation; or
- the beginning of its previous compilation.

The monitor thread tracks the coder's latest compilation start time and checks:

```text
current_time - last_compile_start >= time_to_burnout
```

The burnout event must be logged within **10 ms of the actual burnout time**, as required by the subject.

The monitor is a separate thread rather than relying on the coder thread to detect its own burnout. This is important because a coder may be blocked waiting for shared dongles and therefore cannot reliably monitor its own deadline.

### Burnout and simulation termination

The subject defines two possible termination conditions:

1. **Burnout:** if a coder misses its compilation deadline, the monitor detects the burnout and the simulation must stop.
2. **Successful completion:** if every coder has completed at least `number_of_compiles_required` compilations, the simulation stops normally.

Burnout therefore has priority over normal continued simulation. Once burnout is detected, the shared simulation-stop state must cause the coder threads to leave their routines and the main thread must wait for them before cleanup.

### Special burnout edge case

A particularly important test case is a very small `time_to_burnout`, especially when it is close to or smaller than the time required to progress through the simulation.

This is useful for exposing:

- late burnout detection;
- coders continuing after the simulation should have stopped;
- threads remaining blocked on a dongle;
- incorrect `last_compile_start` updates;
- races between the monitor and coder threads;
- cleanup occurring while worker threads are still active.

This case is treated separately from ordinary functional testing because a program can appear correct with generous timings while still having an incorrect shutdown path.

**Current development status:** the implementation is still being hardened around the burnout/shutdown edge case and the single-coder case. These are known focus areas before final submission. The README documents the required behavior rather than claiming that an unfinished edge case has already been solved.

---

# Thread Synchronization Mechanisms

## `pthread_mutex_t`

The implementation uses several mutexes with separate responsibilities.

### Dongle mutex

Each dongle has its own mutex.

It protects:

```text
in_use
available_at
request_heap
```

Only the thread holding the dongle mutex may modify or inspect these shared values as part of an acquisition/release operation.

### Log mutex

The log mutex serializes output:

```text
pthread_mutex_lock(&log_mutex);
printf(...);
pthread_mutex_unlock(&log_mutex);
```

This prevents interleaved messages from multiple coder threads.

### Simulation mutex

The simulation mutex protects:

```text
simulation_over
```

Coder threads can safely check the flag while the monitor thread can safely update it.

### Sequence mutex

The sequence mutex protects the request sequence counter.

Without it, two coders could potentially read the same sequence number before either increments it, breaking FIFO ordering.

## `pthread_cond_t`

Each dongle has a condition variable.

A coder that cannot currently acquire a dongle waits on the dongle's condition variable rather than continuously performing work.

The condition is rechecked after waking because waking up does not itself guarantee that the requested dongle can be granted.

The implementation also uses a timed wait so that a waiting coder can periodically reconsider its conditions, including simulation termination and cooldown.

## Monitor thread

The monitor is independent from the coder routines.

Its responsibilities are:

- check coder burnout deadlines;
- detect the first coder that misses its deadline;
- set the simulation termination state;
- detect when every coder has completed the required number of compilations.

This separates deadline monitoring from the work performed by coder threads.

---

# Program Structure

The project is divided into several source files so that the responsibilities of the simulation remain separated.

```text
coders/
├── Makefile
├── README.md
├── include/
│   └── codexion.h
└── src/
    ├── codexion.c
    ├── init.c
    ├── threads.c
    ├── routine.c
    ├── actions.c
    ├── monitor.c
    ├── mutex.c
    ├── heap.c
    └── utils.c

```

### `codexion.c`

Program entry point and high-level lifecycle:

```text
parse arguments
      ↓
initialize dongles
      ↓
initialize coders
      ↓
initialize shared mutexes
      ↓
set simulation start time
      ↓
create coder + monitor threads
      ↓
join threads
      ↓
cleanup
```

### `init.c`

Initializes the simulation data structures, coders and dongles.

### `threads.c`

Creates and joins coder and monitor threads.

### `routine.c`

Contains the main coder routine and simulation-state checks.

The coder follows the compile → debug → refactor cycle until the simulation ends.

### `actions.c`

Handles acquiring and releasing dongles.

This is where request insertion, scheduler arbitration, cooldown checks and the ordered dongle-acquisition strategy are coordinated.

### `monitor.c`

Contains the independent monitor thread responsible for burnout and normal completion detection.

### `heap.c`

Implements the priority queue used by the dongle arbitration mechanism.

### `mutex.c`

Initializes the shared mutexes.

### `utils.c`

Contains utility functionality such as millisecond timestamps, sleeping and synchronized state logging.
Clean-up: Destroys synchronization objects and frees dynamically allocated memory.

---

# Data Structures

The main structures are:

### `t_data`

Shared simulation state and configuration.

It contains:

- all command-line parameters;
- coder and dongle arrays;
- scheduler mode;
- simulation start time;
- simulation termination state;
- synchronization objects;
- request sequence counter;
- monitor thread.

### `t_coder`

Represents one coder thread.

It contains:

- coder ID;
- compilation count;
- current state;
- `pthread_t`;
- state mutex;
- left/right dongle pointers;
- pointer to shared simulation data;
- last compilation start timestamp.

### `t_dongle`

Represents one shared dongle.

It contains:

- dongle ID;
- mutex;
- condition variable;
- in-use state;
- cooldown timestamp;
- request heap.

### `t_heap`

Array-backed binary min-heap used as the scheduling priority queue.

### `t_heap_node`

Represents one dongle request and stores:

```text
coder
sequence
deadline
```

---

# Development Process

The project was developed progressively rather than starting immediately with the complete simulation.

## Initial study

The problem was first understood through the Dining Philosophers analogy:

```text
coders  → philosophers
dongles → forks
```

This helped identify the central concurrency problems:

- mutual exclusion;
- deadlock;
- starvation;
- shared-resource arbitration;
- timing;
- monitoring.

The project then moved from basic pthread experiments to mutexes, condition variables, data-structure design and finally the full simulation.

## Thread and mutex exercises

Before implementing Codexion, small pthread exercises were used to understand:

- `pthread_create`;
- `pthread_join`;
- passing arguments to threads;
- returning thread results;
- race conditions;
- mutex initialization, locking, unlocking and destruction.

A shared-counter exercise was used to observe the difference between unsynchronized and mutex-protected access.

## Condition variables

Condition-variable exercises focused on:

```text
pthread_cond_init
pthread_cond_wait
pthread_cond_timedwait
pthread_cond_signal
pthread_cond_broadcast
pthread_cond_destroy
```

The producer/consumer and timed-wait concepts were then applied to waiting for dongle availability.

## Heap

The scheduling queue was developed separately before integrating it into the threaded simulation.

The heap was tested conceptually with both:

```text
FIFO → sequence number
EDF  → deadline
```

This reduced the amount of concurrency logic that had to be debugged simultaneously.

## Testing

Several test scripts were developed for:

- compilation;
- argument validation;
- log format;
- normal execution;
- burnout;
- cooldown;
- FIFO and EDF;
- high coder counts;
- repeated execution;
- memory leaks;
- high-contention situations;
- single-coder behavior;
- shutdown behavior.

The project was also repeatedly compiled with:

```text
-Wall -Wextra -Werror -pthread
```

The additional stress tests include runs with up to 200 coders and repeated high-contention executions.

---

# Resources

The following resources were used during the learning and development process.

## Pthreads and concurrency

- CodeVault — pthreads and mutex/condition-variable tutorials:
  https://www.youtube.com/watch?v=d9s_d28yJq0
- CodeVault — mutex introduction:
  https://www.youtube.com/watch?v=raLCgPK-Igc
- CodeVault — pthread/thread examples:
  https://www.youtube.com/watch?v=UGQsvVKwe90
- Introduction to threads:
  https://www.youtube.com/watch?v=ldJ8WGZVXZk
- Udacity / pthread condition variables:
  https://www.youtube.com/watch?v=eQOaaDA92SI
- GeeksforGeeks — pthread condition wait/signal:
  https://www.geeksforgeeks.org/condition-wait-signal-multi-threading/
- University of Kent — Unix/system programming thread study material:
  https://www.cs.kent.edu/~ruttan/sysprog/lectures/multi-thread/multi-thread.html#definition
- Colorado lecture material on monitors and condition variables:
  https://home.cs.colorado.edu/~rhan/CSCI_3753_Spring_2005/CSCI_3753_Spring_2005/Lectures/02_22_05_dp_mon_cv.pdf

## Dining Philosophers and synchronization

The Dining Philosophers problem was used as a conceptual starting point.

- Neso Academy — Dining Philosophers:
  https://www.youtube.com/watch?v=K52NiClfvyE
- Dining Philosophers problem explanation:
  https://www.youtube.com/watch?v=NbwbQQB7xNQ
- CodeLucky — Dining Philosophers:
  https://codelucky.com/dining-philosophers/
- Medium — Dining Philosophers with mutex locks in C:
  https://apoorvasn.medium.com/solving-the-dining-philosophers-problem-with-mutex-locks-and-c-programming-cee5ac8d35e7

## Codexion-specific references

- Codexion Visualizer:
  https://codexion-visualizer.sacha-dev.me/
- Dev.to — Thread in C / Codexion:
  https://dev.to/yel-bakk/thread-in-c-codexion-42-1ao
- Overtekk — Codexion concepts/reference repository:
  https://github.com/Overtekk/Codexion
- Jamshidbek2000 — philosophers reference implementation:
  https://github.com/Jamshidbek2000/philosophers_42

These repositories and examples were used as learning references and for understanding concepts. They were not used as a source for copying the project's implementation.

## Heap resources

The following resources were consulted while learning how to implement an array-backed binary heap in C:

- https://github.com/charJe/heap
- https://gist.github.com/nyorain/468f4450b0b6585ac04100199485cde1
- https://www.youtube.com/watch?v=pAU21g-jBiE
- https://www.youtube.com/watch?v=0wPlzMU-k00

## AI usage

AI tools were used as learning and productivity aids during the project.

They were used for:

- explaining pthread concepts and synchronization;
- explaining mutexes and condition variables;
- discussing Dining Philosophers and concurrency concepts;
- helping break the project into smaller implementation stages;
- discussing possible data structures and synchronization strategies;
- suggesting test cases and edge cases;
- reviewing reasoning about deadlock, starvation and burnout;
- helping identify weaknesses in test scripts;
- improving documentation structure and README organization.

AI-generated material was treated as something to review and test rather than as authoritative code. The implementation was developed incrementally, and the reasoning behind the synchronization mechanisms is intended to be understood and defendable during peer evaluation.

---
