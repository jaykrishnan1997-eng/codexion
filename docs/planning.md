# Codexion — Detailed 10-Day Plan

Assumes ~6–7 hours/day. Each day lists concrete exercises/goals so you know what "done" looks like before moving on.

## Day 1 — Thread fundamentals (6–7h)

**Morning: subject comprehension**
- Read the subject twice, fully. Write in your own words: the coder cycle (compile→debug→refactor→compile), what triggers burnout, both stop conditions, what "fair arbitration" means, what dongle cooldown does.
- Note the allowed-function whitelist somewhere visible (no libft, ~20 functions only — Chapter VI table).

**Afternoon: pthread basics**
- `pthread_create` / `pthread_join` lifecycle.
- Exercise 1: one thread prints "Hello".
- Exercise 2: 5 threads, each prints its own number (pass `int*` or cast via `(void*)(long)i`).
- Exercise 3: each thread does a small calculation, returns a result via `pthread_exit`, main collects it via `pthread_join`'s `void**` output param.
- **Goal check:** explain `pthread_create()` → `pthread_join()` without notes.

## Day 2 — Mutexes + condition variables (6–7h)

**Morning: mutexes**
- Race condition demo: 100 threads incrementing a shared counter 10,000× each.
  - Run without a mutex — observe the final count is wrong.
  - Add `pthread_mutex_init/lock/unlock/destroy` — observe it's now correct every time.
- **Goal check:** instinctively ask "who else can touch this variable right now?" for any shared data.

**Afternoon: condition variables**
- `pthread_cond_init/wait/signal/broadcast/destroy`.
- Toy exercise: producer/consumer — one thread waits on a condition (`pthread_cond_wait`) until a flag is set, another thread sets the flag and signals.
- Then try `pthread_cond_timedwait` specifically: make a thread wait either until signaled *or* until a deadline (`gettimeofday` + offset, converted to `struct timespec`). This exact pattern is what your monitor thread will use.
- **Goal check:** explain why `pthread_cond_wait` is better than a `while(1){ usleep(...); check flag; }` busy-loop, and what `timedwait` buys you over plain `wait`.

## Day 3 — Design day (6–7h)

No code today except throwaway sketches/pseudocode. This is the day that saves you the most time later.

**Structs to sketch:**
- `t_sim` — shared config (all 8 args), array of `t_coder`, array of `t_dongle`, scheduler mode, log mutex, global "stop" flag + its mutex/cond, simulation start time.
- `t_coder` — id, `pthread_t`, current state, compiles_done count, `last_compile_start` timestamp, pointers to left/right dongle.
- `t_dongle` — mutex, state (available/held/cooling), `available_at` timestamp (for cooldown), a waiting-request structure feeding into the scheduler.
- Heap node — request info: coder id, arrival sequence number (for FIFO), deadline (for EDF), pointer back to the dongle or request context.

**Decisions to make and write down (even briefly):**
- Which data is shared across threads vs. local to one thread → which needs a mutex.
- The heap: one comparator function, or two (FIFO vs EDF), selected by a function pointer or a mode flag.
- Monitor thread's exact job: loop over coders, check `now - last_compile_start >= time_to_burnout`? Or use `pthread_cond_timedwait` per-coder-deadline to sleep efficiently instead of busy-polling? (The subject wants precision — the timedwait approach is worth designing for over a tight poll loop.)
- Dongle acquisition order: how do you avoid two coders deadlocking each holding one of a pair? Decide your strategy now (e.g. always acquire lower-indexed dongle first) — write the reasoning down, you'll need it for the README anyway.
- Who prints logs, and how you guarantee no interleaving (one global log mutex, held only for the `printf`/`fprintf` call itself).

**Goal check:** you should be able to describe the full data flow from `main()` to a coder logging "is compiling" without hand-waving any step.

## Day 4 — Skeleton + heap (standalone) (6–7h)

**Skeleton (first half of day):**
- Argument parsing + strict validation: reject negative numbers, non-integers, wrong argument count, scheduler string not exactly `fifo`/`edf`.
- Init: structs, mutexes, cond vars.
- Create N coder threads + 1 monitor thread, each with an empty/stub routine.
- Join all threads, destroy mutexes/cond vars, free everything.
- **Goal:** `./codexion 1 800 200 100 100 3 50 fifo` runs and exits cleanly with zero real logic yet — valgrind/leak-clean at this stage already.

**Heap (second half of day) — build standalone, no threads involved:**
- Implement insert and extract-min on an array-backed binary heap.
- Write a FIFO comparator (by arrival sequence number) and an EDF comparator (by deadline).
- Test with a hardcoded array of fake requests (not threads) — confirm both comparators pop in correct order.
- **Goal:** you trust the heap in isolation before you ever plug real threads into it — this makes Day 5 debugging much easier.

## Day 5 — Dongle acquisition + cooldown (6–7h)

- Wire real coder threads to request their left+right dongles through the scheduler/heap.
- Implement dongle state machine, mutex-protected: `available → held → released → cooling_down (available_at = now + cooldown) → available`.
- On request: coder pushes itself onto the relevant heap(s)/queue, waits (via cond var) until granted both dongles.
- Enforce cooldown strictly: a dongle must not be grantable while `now < available_at`, even if it's otherwise "free" and next in the FIFO/EDF order.
- Test with 2 coders first — watch the raw log output, confirm dongles alternate sensibly and no coder ever appears to hold a dongle during another's compile window.
- Scale to 3–5 coders. Specifically test: does a coder ever "win" arbitration for a dongle still cooling down? (This is the bug class most specific to this subject — hunt for it explicitly.)

## Day 6 — Burnout timing + monitor thread (6–7h)

This is the day to not rush.

- Implement `last_compile_start` updates correctly (set the instant a coder *begins* compiling, i.e. once it holds both dongles).
- Build the monitor thread as genuinely separate: it should track every coder's deadline (`last_compile_start + time_to_burnout`, or simulation start for the first cycle) and detect the first coder to cross it.
- Prefer `pthread_cond_timedwait` on a shared cond var with a computed "next deadline" rather than tight polling — polling can miss the 10ms window under load or waste CPU.
- On detecting burnout: log within 10ms, set a global stop flag, and make sure all coder threads notice it and exit their loops promptly (check the flag at each phase transition).
- Test deliberately with tight timing values (e.g. `time_to_burnout` close to `time_to_compile + time_to_debug + time_to_refactor`) to force burnouts and check the log timestamp is honest.
- Confirm the log mutex prevents any interleaved/garbled lines even under this stress.

## Day 7 — Deadlock/starvation reasoning + full simulation (6–7h)

**Morning — reasoning (don't skip this even though it's "just theory"):**
- Write out Coffman's four conditions and check each against your dongle-acquisition design: mutual exclusion (yes, inherent), hold-and-wait (are you acquiring both dongles atomically or one-then-wait-for-other?), no preemption, circular wait (does your acquisition order break this?).
- Confirm your EDF implementation actually prevents starvation as required — reason through a concrete scenario with 3+ coders where FIFO might starve someone but EDF shouldn't.

**Afternoon — full cycle:**
- Wire compile → debug → refactor → release dongles → re-attempt compile, incrementing `compiles_done`.
- Implement the second stop condition: sim ends cleanly (not via burnout) once every coder has hit `number_of_compiles_required`.
- **Goal:** a full run matching the subject's example log format exactly, for both schedulers.

## Day 8 — Edge cases + torture testing (6–7h)

**Edge cases (first half):**
- 1 coder (single dongle case — explicitly called out in the subject, not a pair).
- 2 coders, then a larger N (10+).
- Extreme timing: `time_to_compile` close to `time_to_burnout`, `dongle_cooldown = 0`, very large cooldown.
- Invalid args: negative numbers, non-integers, missing args, extra args, bad scheduler string — confirm clean rejection, no crash.
- Run identical scenarios under both `fifo` and `edf`, confirm behavior differs sensibly.

**Torture testing (second half):**
- `for i in {1..200}; do ./codexion <args>; done` — watch for hangs, wrong log ordering, crashes.
- Run with a thread sanitizer / address sanitizer if your toolchain supports it — this is exactly the kind of bug class visual inspection misses.
- Check specifically: every mutex unlocked by the thread that locked it, no double-locks, all threads actually terminate (no hangs on join).

## Day 9 — Cleanup, Norm, README (6–7h)

- Norm-check every file (mandatory and any bonus files — bonus errors zero the whole project).
- Confirm clean compile with `-Wall -Wextra -Werror -pthread`, no warnings.
- Final leak check (valgrind or equivalent).
- Write `README.md`:
  - First line, italicized, exact format: attribution to your login(s).
  - **Description** — goal + brief overview.
  - **Instructions** — compile/run.
  - **Resources** — your references + explicit AI-usage disclosure (what you asked AI for, which parts).
  - **Blocking cases handled** — deadlock prevention (reference Coffman's conditions and your specific strategy), starvation prevention (why EDF doesn't starve), cooldown handling, precise burnout detection, log serialization.
  - **Thread synchronization mechanisms** — your mutexes, cond vars, how they coordinate dongles/logging/monitor state, with a concrete example of a race condition you prevented.

## Day 10 — Final validation + defense prep (6–7h)

- Fresh clone of your repo, compile exactly as graded, run cold — no reliance on leftover build artifacts.
- Deliberately try to break it once more: all threads joined? all mutexes/cond vars destroyed? every malloc freed? can two threads ever touch the same dongle simultaneously? does burnout fire within 10ms consistently across repeated runs?
- Rehearse out loud: why you chose your data structures, how FIFO vs EDF differ concretely in your code, how cooldown is enforced, how deadlock is prevented — you'll likely be asked to justify these live, and possibly do a small "recode" to prove understanding.

---

**Pacing note:** Days 6 and 7 are the most likely to run long since that's where real concurrency bugs surface. If they do, protect Day 9 (README/cleanup) rather than Day 8 (torture testing) — a rushed README costs you documentation points, but skipped torture testing risks a hang during your actual defense, which is worse.