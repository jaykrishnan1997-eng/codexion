# THIS IS CODEXION

### DAY 0[Aug 31]

- Understand basics from what is philosophers dining, forks, etc
- From claude, references: it's essentially 42's newer "coders competing for dongles" spin on the classic Dining Philosophers problem: multiple threads racing for a limited shared resource pool, using pthread mutexes/condition variables, with FIFO or EDF scheduling and starvation/deadlock as the core danger.

1. Concept first — the Dining Philosophers problem itself
This is the theory Codexion is built on (coders = philosophers, dongles = forks).

    "Dining Philosophers Problem with Solution" – https://www.youtube.com/watch?v=NbwbQQB7xNQ
    Neso Academy, "Dining Philosophers Solution using Monitors" – https://www.youtube.com/watch?v=K52NiClfvyE (good for understanding deadlock/starvation avoidance conceptually before touching code)

2. pthreads basics in C

    "Mutex Introduction (pthreads) | C Programming Tutorial" – https://www.youtube.com/watch?v=raLCgPK-Igc (Portfolio Courses — clean, C-specific, shows race conditions and how mutexes fix them)
    https://www.youtube.com/watch?v=oq29KUy29iQ&pp=ugUEEgJlbg%3D%3D: code vault mutex

3. Condition variables (you'll need these for coordinating dongle availability/wakeups)

    "Pthread Condition Variables" (Udacity OS course clip) – https://www.youtube.com/watch?v=eQOaaDA92SI

4. Reference reading once the above clicks

    The GeeksforGeeks writeups on pthread_cond_wait/pthread_cond_signal are good for pinning down exact syntax when you start coding: https://www.geeksforgeeks.org/condition-wait-signal-multi-threading/

    A note on scheduling: Codexion's FIFO vs EDF scheduler argument is the part that goes beyond classic Philosophers — EDF (Earliest Deadline First) is a real-time scheduling algorithm. If your subject actually requires implementing EDF logic, it's worth a separate 10-minute search on "Earliest Deadline First scheduling explained" once you're past the threading basics, since that's a distinct concept from the mutex/condvar stuff above.

    Suggested order: watch #1 and #2 first (understand why the problem is hard), then #3 (the actual pthread mechanics), then go straight to writing code — this is a project where reading the subject PDF closely and experimenting will teach you more than more videos will.

    Those visualizer tools are overkill/off-target for what you need. Here's a more useful, curated list — mixing written guides, a visual tool, and reference material specific to this kind of project:

    Written walkthroughs (often clearer than video for this topic)

    The dev.to "Thread in C (Codexion 42)" article you'll find alongside my earlier search — it's a chapter-by-chapter breakdown written specifically for this project, covering what pthread_create actually does under the hood, what's shared vs private between threads, and timing (wall clock vs monotonic). Worth reading slowly: https://dev.to/yel-bakk/thread-in-c-codexion-42-1ao
    "Solving the Dining Philosophers Problem with Mutex Locks and C Programming" (Medium) — walks through actual C code structured like a 42-style philosophers project (t_rcs structs, monitor thread, etc.), close to what Codexion needs: https://apoorvasn.medium.com/solving-the-dining-philosophers-problem-with-mutex-locks-and-c-programming-cee5ac8d35e7
    CodeLucky's "Dining Philosophers: Classic Concurrency Problem Explained Visually and Practically" — has diagrams showing deadlock and starvation happening step by step: https://codelucky.com/dining-philosophers/

    A tool built for exactly this project

    Codexion Visualizer — an interactive tool made for 42 students specifically to watch coders/dongles/scheduler behavior in real time: https://codexion-visualizer.sacha-dev.me/
    This is probably your single best resource: you can watch FIFO vs EDF scheduling play out visually before you write a line of C.

    Reference repos (read, don't copy)

    Overtekk/Codexion on GitHub explains the concepts in its README without sharing the actual subject PDF or graded code, which respects 42 rules but still teaches the ideas: https://github.com/Overtekk/Codexion

    My honest suggestion given how you described feeling about Fly-in: start with the Codexion Visualizer link above and just play with the parameters for 15 minutes before reading anything else. Seeing the starvation/deadlock happen on screen tends to make the "why do I need a mutex here" question answer itself.

## DAY 1 [Sept 1]

- Learning about pthreads, how to use them , how they work etc:
    1. A done example and yt explanation:
        https://www.youtube.com/watch?v=UGQsvVKwe90
        https://github.com/Jamshidbek2000/philosophers_42
    2. A good deep resource book on thread: https://github.com/freebendy/ben-books/tree/master
    3. Introduction to thread: https://www.youtube.com/watch?v=ldJ8WGZVXZk
    4. Mutex introduction : https://www.youtube.com/watch?v=raLCgPK-Igc 
    5. Threads in C: https://www.youtube.com/watch?v=d9s_d28yJq0&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2
    6. Study Guide: https://www.cs.kent.edu/~ruttan/sysprog/lectures/multi-thread/multi-thread.html#definition
    7. CV Unix Thread in C: https://www.youtube.com/watch?v=d9s_d28yJq0 list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2

## DAY 2 [Sept 2]

- Reading through i get an idea that Monitor Pattern with Condition Variables. is the best solution for N number of philosophers and forks.

    1. Monitors and conditional variables: https://home.cs.colorado.edu/~rhan/CSCI_3753_Spring_2005/CSCI_3753_Spring_2005/Lectures/02_22_05_dp_mon_cv.pdf
    2. before which lets see a video on all these: https://www.youtube.com/watch?v=d9s_d28yJq0&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2&index=1
    
## DAY 3 [Sept 3]

- Tried some more videos and coding to understand what pthread, mutex etx are and why they are necessary. Also more focus on the return values from thread and passing arguments. All thanks to codevault videos.

## DAY 4 [Sept 4]
- todays work:

    1. Starting with structures after reading the subject carefully. Also must do a simple parsing start.

    2. PLAN:

            main.c
                ↓
            starts the program

            init.c
                ↓
            creates/initializes everything

            threads.c
                ↓
            starts/stops threads

            routine.c
                ↓
            what each coder actually does

            actions.c
                ↓
            compile/debug/refactor behaviour

            monitor.c
                ↓
            checks whether coders are burning out

            cleanup.c
                ↓
            destroys everything

    3. I am planning to make a linked list for the request since i dont habve request for that. Its nice if i could also hold a stat for coder. Like C, L, R, where C is the coder, L is left dongle and R is right one. Problem must L AND R be static or dynamic that appears as per request and needs.

