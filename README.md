*This project has been created as part of the 42 curriculum by jkrishna.*

# Codexion

## Description

Codexion is a concurrency and synchronization project written in C as part of the 42 curriculum. It is a themed variant of the classic Dining Philosophers problem.

The simulation models a group of coders working around a shared quantum compiler. Each coder is a POSIX thread that repeatedly cycles through:

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

The dongles are shared resources: a coder can compile only while holding both of its two required dongles. Multiple coders may request the same dongle at the same time, so access is synchronized and granted according to a selectable scheduling policy.

The program supports two arbitration policies:

- **FIFO** — First In, First Out. A dongle serves whichever pending request arrived first.
- **EDF** — Earliest Deadline First. A dongle serves whichever pending coder has the closest burnout deadline:

```text
deadline = last_compile_start + time_to_burnout
```

The project combines:

- POSIX threads (`pthread_create()` / `pthread_join()`)
- mutexes and condition variables
- a hand-written binary min-heap used as a priority queue
- FIFO and EDF scheduling on top of the same heap
- shared-resource acquisition with cooldown
- an independent monitor thread for burnout detection
- millisecond-precision timing
- mutex-protected logging
- dynamic memory management and cleanup

There are no global variables — all shared state lives in `t_data` and is passed explicitly.

---

## Program Requirements

The executable is named:

```text
codexion
```

It takes exactly eight mandatory arguments (nine `argv` entries counting the program name):

```text
./codexion number_of_coders time_to_burnout time_to_compile \
          time_to_debug time_to_refactor number_of_compiles_required \
          dongle_cooldown scheduler
```

### Arguments

| Argument | Description |
|---|---|
| `number_of_coders` | Number of coder threads and, for more than one coder, number of dongles |
| `time_to_burnout` | Maximum time a coder may go without starting another compilation |
| `time_to_compile` | Time spent compiling while holding both dongles |
| `time_to_debug` | Time spent debugging after compilation |
| `time_to_refactor` | Time spent refactoring after debugging |
| `number_of_compiles_required` | Number of successful compilations required from every coder before normal termination |
| `dongle_cooldown` | Time a released dongle remains unavailable (may be `0`) |
| `scheduler` | `fifo` or `edf` |

All times are expressed in milliseconds.

Invalid arguments are rejected: a missing/extra argument, a non-numeric value, a negative or zero value where a positive one is required, and any scheduler other than exactly `fifo` or `edf`.

---

## Instructions

### Compilation

```bash
make
```

The Makefile compiles with:

```text
cc -Wall -Wextra -Werror -pthread
```

Other targets:

```bash
make all
make clean
make fclean
make re
```

### Execution

FIFO example:

```bash
./codexion 3 2000 200 100 100 3 50 fifo
```

EDF example:

```bash
./codexion 3 2000 200 100 100 3 50 edf
```

The scheduler argument must be exactly `fifo` or `edf`.

---

## Simulation Rules

### Coders

Every coder is a `pthread_t`. Internally coder indices run `0 .. number_of_coders - 1`, but they are logged as `1 .. number_of_coders` so the output matches the subject's numbering.

The coders sit in a circle:

```text
1 -- 2 -- 3 -- ... -- N
|                   |
+-------------------+
```

Coder `1` is adjacent to coder `N`. Each coder has a left and a right dongle, and each dongle (except in the single-coder case) is shared by exactly two adjacent coders.

**Single-coder case:** when `number_of_coders` is `1`, there is only one dongle, and the coder's left and right pointers both refer to it. The two-dongle acquisition protocol is not special-cased away: the coder takes the dongle once, logs `has taken a dongle`, then attempts to take the *same* dongle again as its second required dongle — which can never succeed, since nothing else will ever release it. This is intentional: it demonstrates that the burnout monitor correctly detects a coder that can make no further progress. With `number_of_coders 1`, the simulation therefore always ends in burnout rather than successful completion, regardless of how large `time_to_burnout` is set — a larger value only delays when the burnout fires.

### Compilation

A coder must acquire both of its dongles before compiling. The log therefore contains two `has taken a dongle` lines immediately before `is compiling`.

After compilation, both dongles are released (entering cooldown) before the coder debugs and refactors.

### Debugging and refactoring

After compiling, a coder:

1. releases both dongles;
2. debugs;
3. refactors;
4. attempts to acquire its dongles again.

---

## Scheduling

### FIFO

Each dongle request receives a monotonically increasing sequence number, handed out under a dedicated mutex so concurrent requests can't collide. Under FIFO, the dongle's request heap is ordered by that sequence number, so the request that arrived first is served first.

### EDF

Under EDF, the request heap is ordered by burnout deadline instead:

```text
deadline = last_compile_start + time_to_burnout
```

Each dongle serves the pending request for that dongle with the closest burnout deadline, regardless of arrival order.

The heap implementation itself is identical for both modes — only the comparison rule (`is_smaller`) changes based on the `scheduler` string.

---

## Priority Queue / Heap

Codexion implements its own array-backed binary min-heap, since C has no built-in priority queue.

Each heap node stores:

```text
coder
sequence
deadline
```

Supported operations:

- `heap_insert` — sift-up insertion
- `heap_extract_min` — removes the root, moves the last element to the root, and sifts it down

Each dongle owns its own heap, and the dongle's mutex protects both the dongle's state and its heap.

---

## Dongle Cooldown

A released dongle isn't immediately reusable. On release:

```text
available_at = current_time + dongle_cooldown
```

A coder can only acquire a dongle once **all** of the following hold:

- the dongle is not currently in use;
- its cooldown has expired (`current_time >= available_at`);
- the coder is at the front of that dongle's request heap (highest priority under FIFO or EDF).

Waiting coders block on the dongle's condition variable and are woken on release; the wait uses `pthread_cond_timedwait` (with a short periodic re-check) rather than a plain `pthread_cond_wait`, so a coder also re-evaluates simulation-termination and cooldown state even without an explicit signal.

---

# Blocking Cases Handled

## Deadlock

The classic circular-wait risk — two adjacent coders each holding one dongle and waiting on the other — is avoided by imposing a fixed acquisition order:

```text
acquire the lower-ID dongle first
acquire the higher-ID dongle second
```

Because every coder compares its own left/right dongle IDs before acquiring, no cycle of "holds A, waits for B" / "holds B, waits for A" can form. This targets Coffman's **circular-wait** condition specifically; the other three classic conditions —

1. Mutual exclusion
2. Hold and wait
3. No preemption
4. Circular wait

— are either unavoidable (mutual exclusion: a dongle can only be used by one coder) or not addressed by this ordering trick and aren't separately broken here.

The single-coder case is worth calling out separately, since a lone coder's left and right dongle pointers refer to the same physical dongle: it can never truly acquire "two" dongles — after taking it once, its second request for that same, now-held, dongle can never be granted. Rather than special-casing this away, the simulation lets it play out naturally. This is deadlock in the strict sense (the coder can never proceed), but the independent monitor thread still detects it via burnout and shuts the simulation down cleanly instead of hanging forever.

## Resource duplication

Each dongle has its own `pthread_mutex_t` guarding its `in_use` flag, `available_at` timestamp, and request heap, so two coders can never simultaneously believe they hold the same dongle.

## Starvation

FIFO guarantees arrival-order service. EDF instead prioritizes the coder closest to burnout, so a coder that keeps losing arbitration under FIFO would instead be prioritized as its deadline approaches under EDF.

## Cooldown blocking

A dongle that was just released stays unavailable until `available_at` is reached, even for the coder with the highest-priority pending request — cooldown is checked independently of scheduling priority in `wait_for_dongle`.

## Logging races

All output goes through `log_state`, which serializes printing under `log_mutex`, so two coders' log lines can never interleave on the same line. Format:

```text
timestamp coder_id message
```

Example:

```text
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
201 1 is debugging
401 1 is refactoring
```

## Shared simulation state

- `simulation_over` is guarded by `sim_mutex` and checked by every coder thread and the monitor thread.
- The sequence counter used for FIFO ordering is guarded by `sequence_mutex`.
- Each coder's own mutable state (`no_of_compiles`, `last_compile_start`) is guarded by that coder's own `state_mutex`.

---

# Burnout Detection

A coder burns out if it hasn't started a new compilation within `time_to_burnout` ms of either the simulation start or its previous compilation start.

A separate **monitor thread** — not the coder threads themselves — polls every 3 ms and checks, for each coder:

```text
current_time - last_compile_start >= time_to_burnout
```

The monitor is deliberately independent of the coder routines: a coder can be blocked waiting on a dongle and therefore isn't in a position to reliably watch its own deadline.

### Termination conditions

The monitor stops the simulation (sets `simulation_over`) as soon as either is true:

1. **Burnout** — any coder has missed its deadline.
2. **Successful completion** — every coder has reached `number_of_compiles_required` compilations.

### Prompt shutdown mid-cycle

Each phase (compile, debug, refactor) sleeps via `interruptible_sleep`, which breaks the requested duration into small chunks and re-checks `simulation_over` between chunks instead of sleeping through the whole phase in one call. A coder that is mid-compile, mid-debug, or mid-refactor when the monitor sets `simulation_over` — whether from a burnout or from every coder reaching its required compile count — stops within a couple of milliseconds rather than finishing the rest of its current cycle. If it was interrupted mid-compile, it releases its dongles before exiting rather than leaving them marked in use.

This was verified directly: with a tight `time_to_burnout`, the last log line printed by any coder is now the `burned out` line itself, across repeated runs and coder counts up to 200.

---

# Thread Synchronization Mechanisms

## `pthread_mutex_t`

| Mutex | Scope | Protects |
|---|---|---|
| Dongle mutex (per dongle) | `t_dongle` | `in_use`, `available_at`, `request_heap` |
| `log_mutex` | `t_data` | Interleaving of `printf` calls |
| `sim_mutex` | `t_data` | `simulation_over` |
| `sequence_mutex` | `t_data` | `next_sequence` (FIFO ordering) |
| State mutex (per coder) | `t_coder` | `no_of_compiles`, `last_compile_start` |

## `pthread_cond_t`

Each dongle has its own condition variable. A coder that cannot yet acquire a dongle waits on it via `pthread_cond_timedwait` rather than busy-looping, and re-checks its acquisition condition every time it wakes (whether from a signal or from the timeout), since a wake-up alone doesn't guarantee the dongle is actually available.

## Monitor thread

Runs independently of the coder threads. Responsibilities:

- poll every coder's burnout deadline;
- detect the first burnout;
- detect full completion (every coder reaching the required compile count);
- set `simulation_over` when either condition is met.

---

# Program Structure

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
    ├── take_dongles.c
    ├── release_dongle.c
    ├── monitor.c
    ├── mutex.c
    ├── heap.c
    └── utils.c
```

### `codexion.c`

Entry point and lifecycle:

```text
check argc
      ↓
parse arguments
      ↓
initialize dongles, coders, mutexes
      ↓
record simulation start time
      ↓
create coder threads + monitor thread
      ↓
join all threads
      ↓
cleanup
```

### `init.c`

Argument parsing/validation (`parser`, `validate_numbers`, `is_valid_number`) and initialization of the dongle array and coder array (`init_dongles`, `init_coders`).

### `threads.c`

Creates all coder threads plus the monitor thread (`create_threads`) and joins them all at shutdown (`wait_for_threads`).

### `routine.c`

The coder's main loop (`coder_routine`): take dongles → compile → debug → refactor → repeat, until `is_simulation_over` is true. Each phase sleeps via `interruptible_sleep` rather than a single `usleep`, so the coder can exit mid-phase the moment `simulation_over` is set.

### `take_dongles.c`

Dongle acquisition: builds a request (sequence number + deadline), inserts it into the target dongle's heap, waits until it's at the front of the heap **and** the dongle is free and off cooldown, then marks it in use. Handles both the ordered-acquisition case (two distinct dongles) and the single-dongle case.

### `release_dongle.c`

Dongle release: sets the new cooldown timestamp, marks the dongle free, and wakes any waiting coders via `pthread_cond_broadcast`.

### `monitor.c`

The independent monitor thread: `check_burnout`, `check_all_done`, and the polling loop that sets `simulation_over`.

### `heap.c`

The binary min-heap used for dongle request arbitration: `heap_insert`, `heap_extract_min`, and the shared `is_smaller` comparison (sequence number for FIFO, deadline for EDF).

### `mutex.c`

Initializes and destroys the simulation-wide mutexes (`log_mutex`, `sim_mutex`, `sequence_mutex`).

### `utils.c`

Millisecond timestamps (`get_time`), the chunked `interruptible_sleep` used by every coder phase, synchronized logging (`log_state`), and final cleanup (`cleanup`) — destroying synchronization primitives and freeing all allocated memory.

---

# Data Structures

### `t_data`

Shared simulation state and configuration: all command-line parameters, the coder and dongle arrays, the scheduler mode, simulation start time, the termination flag, the shared synchronization objects, the sequence counter, and the monitor thread handle.

### `t_coder`

One coder thread: coder ID, compile count, current state string, `pthread_t`, its own state mutex, pointers to its left/right dongles, a back-pointer to the shared `t_data`, and its last compile start timestamp.

### `t_dongle`

One shared dongle: dongle ID, mutex, condition variable, in-use flag, cooldown timestamp, and its own request heap.

### `t_heap` / `t_heap_node`

Array-backed binary min-heap and its node (`coder`, `sequence`, `deadline`), used identically by both scheduling modes with only the comparison rule differing.

---

# Development Process

Developed incrementally rather than starting with the full simulation at once.

## Initial study

The problem was first understood through the Dining Philosophers analogy (`coders → philosophers`, `dongles → forks`), which surfaced the core concurrency problems: mutual exclusion, deadlock, starvation, shared-resource arbitration, timing, and monitoring.

## Thread and mutex exercises

Small standalone exercises were used first to get comfortable with `pthread_create`, `pthread_join`, passing/returning thread arguments, observing race conditions, and mutex init/lock/unlock/destroy — including a shared-counter exercise contrasting unsynchronized vs. mutex-protected access.

## Condition variables

Separate exercises covered `pthread_cond_init/wait/timedwait/signal/broadcast/destroy`, using a producer/consumer pattern before applying the same ideas to waiting for dongle availability.

## Heap

The priority queue was built and reasoned about on its own — first for FIFO (sequence number), then for EDF (deadline) — before being wired into the threaded simulation, to avoid debugging heap logic and concurrency logic at the same time.

## Testing

Test scripts covered: compilation, argument validation, log format, normal execution, burnout, cooldown, both schedulers, high coder counts (up to 200), repeated runs, memory leaks, high-contention scenarios, the single-coder case, and shutdown behavior — with every build run through `-Wall -Wextra -Werror -pthread`.

---

# Resources

## Pthreads and concurrency

- CodeVault — pthreads and mutex/condition-variable tutorials: https://www.youtube.com/watch?v=d9s_d28yJq0
- CodeVault — mutex introduction: https://www.youtube.com/watch?v=raLCgPK-Igc
- CodeVault — pthread/thread examples: https://www.youtube.com/watch?v=UGQsvVKwe90
- Introduction to threads: https://www.youtube.com/watch?v=ldJ8WGZVXZk
- Udacity — pthread condition variables: https://www.youtube.com/watch?v=eQOaaDA92SI
- GeeksforGeeks — pthread condition wait/signal: https://www.geeksforgeeks.org/condition-wait-signal-multi-threading/
- University of Kent — Unix/system programming thread study material: https://www.cs.kent.edu/~ruttan/sysprog/lectures/multi-thread/multi-thread.html#definition
- Colorado lecture material on monitors and condition variables: https://home.cs.colorado.edu/~rhan/CSCI_3753_Spring_2005/CSCI_3753_Spring_2005/Lectures/02_22_05_dp_mon_cv.pdf

## Dining Philosophers and synchronization

- Neso Academy — Dining Philosophers: https://www.youtube.com/watch?v=K52NiClfvyE
- Dining Philosophers problem explanation: https://www.youtube.com/watch?v=NbwbQQB7xNQ
- CodeLucky — Dining Philosophers: https://codelucky.com/dining-philosophers/
- Medium — Dining Philosophers with mutex locks in C: https://apoorvasn.medium.com/solving-the-dining-philosophers-problem-with-mutex-locks-and-c-programming-cee5ac8d35e7

## Codexion-specific references

- Codexion Visualizer: https://codexion-visualizer.sacha-dev.me/
- Dev.to — Thread in C / Codexion: https://dev.to/yel-bakk/thread-in-c-codexion-42-1ao
- Overtekk — Codexion concepts/reference repository: https://github.com/Overtekk/Codexion
- Jamshidbek2000 — philosophers reference implementation: https://github.com/Jamshidbek2000/philosophers_42

These were used as learning references only, not as a source for copying the implementation.

## Heap resources

- https://github.com/charJe/heap
- https://gist.github.com/nyorain/468f4450b0b6585ac04100199485cde1
- https://www.youtube.com/watch?v=pAU21g-jBiE
- https://www.youtube.com/watch?v=0wPlzMU-k00

## AI usage

AI tools were used as learning and productivity aids: explaining pthread/mutex/condition-variable concepts, discussing the Dining Philosophers analogy, breaking the project into implementation stages, discussing data structure and synchronization strategy options, providing information on resources to learn from, suggesting test cases and edge cases, reviewing reasoning about deadlock/starvation/burnout, identifying weaknesses in test scripts, and helping structure this documentation.

---

