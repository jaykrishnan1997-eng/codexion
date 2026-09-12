#!/bin/bash
#
# test_codexion_extra.sh
#
# EXTRA tests for Codexion.
#
# This file is intentionally separate from the main test script.
# It focuses on cases that are NOT covered by the usual compilation,
# basic argument validation, normal easy/medium cases, valgrind, and
# repeated 3-coder torture tests.
#
# Run from the project root:
#     chmod +x test_codexion_extra.sh
#     ./test_codexion_extra.sh
#

BINARY="./codexion"
LOG_REGEX='^[0-9]+ [0-9]+ (has taken a dongle|is compiling|is debugging|is refactoring|burned out)$'

PASS=0
FAIL=0

green()  { printf '\033[32m%s\033[0m\n' "$1"; }
red()    { printf '\033[31m%s\033[0m\n' "$1"; }
yellow() { printf '\033[33m%s\033[0m\n' "$1"; }

pass() { PASS=$((PASS + 1)); green "  PASS: $1"; }
fail() { FAIL=$((FAIL + 1)); red   "  FAIL: $1"; }

section() {
    printf '\n\033[1m== %s ==\033[0m\n' "$1"
}

# ---------------------------------------------------------------------------
section "1. Single-coder tests"
# ---------------------------------------------------------------------------
# The original script starts with 2 coders. A single coder is useful for
# finding assumptions in synchronization and termination logic.

run_single_coder() {
    local sched="$1"
    local logfile="/tmp/codexion_extra_single_${sched}.log"

    timeout 15 "$BINARY" 1 2000 100 100 100 3 50 "$sched" \
        > "$logfile" 2>&1

    local status=$?

    if [ $status -eq 124 ]; then
        fail "single coder ($sched): timed out"
        return
    fi

    if [ $status -ne 0 ]; then
        fail "single coder ($sched): exit status $status"
        return
    fi

    if [ ! -s "$logfile" ]; then
        fail "single coder ($sched): empty log"
        return
    fi

    if ! grep -qE '^([0-9]+) 1 ' "$logfile"; then
        fail "single coder ($sched): coder 1 never appeared"
        return
    fi

    if grep -q "burned out" "$logfile"; then
        fail "single coder ($sched): unexpected burnout"
        return
    fi

    pass "single coder ($sched): completed correctly"
}

run_single_coder fifo
run_single_coder edf

# ---------------------------------------------------------------------------
section "2. Coder-ID coverage"
# ---------------------------------------------------------------------------
# Every coder should be represented in the log.
#
# This catches:
#   - failed thread creation
#   - broken coder IDs
#   - scheduler starvation
#   - a worker that never enters the simulation

check_all_coders() {
    local logfile="$1"
    local coders="$2"

    for ((i = 1; i <= coders; i++)); do
        if ! grep -qE "^[0-9]+ $i " "$logfile"; then
            fail "coder coverage: coder $i never appeared"
            return 1
        fi
    done

    return 0
}

for coders in 10 50 100; do
    logfile="/tmp/codexion_extra_ids_${coders}.log"

    timeout 20 "$BINARY" "$coders" 3000 100 100 100 3 50 fifo \
        > "$logfile" 2>&1

    status=$?

    if [ $status -eq 124 ]; then
        fail "coder coverage ($coders coders): timeout"
        continue
    fi

    if [ $status -ne 0 ]; then
        fail "coder coverage ($coders coders): exit status $status"
        continue
    fi

    if ! grep -qE "$LOG_REGEX" "$logfile"; then
        fail "coder coverage ($coders coders): no valid log lines"
        continue
    fi

    if check_all_coders "$logfile" "$coders"; then
        pass "coder coverage ($coders coders): every coder appeared"
    fi
done

# ---------------------------------------------------------------------------
section "3. Maximum allowed coder count / high contention"
# ---------------------------------------------------------------------------
# The original script does not test 100 or 200 coders.
#
# Many pthread bugs only become visible when a large number of threads
# compete for a small number of mutex-protected resources.

run_high_contention() {
    local coders="$1"
    local sched="$2"
    local logfile="/tmp/codexion_extra_contention_${coders}_${sched}.log"

    timeout 25 "$BINARY" "$coders" 1500 100 100 100 3 10 "$sched" \
        > "$logfile" 2>&1

    local status=$?

    if [ $status -eq 124 ]; then
        fail "high contention ($coders, $sched): timeout/deadlock"
        return
    fi

    if [ $status -ne 0 ]; then
        fail "high contention ($coders, $sched): exit status $status"
        return
    fi

    if [ ! -s "$logfile" ]; then
        fail "high contention ($coders, $sched): empty log"
        return
    fi

    if ! grep -qE "$LOG_REGEX" "$logfile"; then
        fail "high contention ($coders, $sched): malformed/no valid log"
        return
    fi

    pass "high contention ($coders, $sched): completed"
}

run_high_contention 100 fifo
run_high_contention 100 edf
run_high_contention 200 fifo
run_high_contention 200 edf

# ---------------------------------------------------------------------------
section "4. Dongle-count stress"
# ---------------------------------------------------------------------------
# Keep the other parameters identical and change ONLY the number of dongles.
# This stresses resource contention and the path where coders must wait.
#
# 1 dongle = extreme contention
# 2 dongles = strong contention
# 3 dongles = normal comparison
#
# If your subject has a documented minimum/maximum for this parameter,
# keep the values within those limits.

run_dongle_case() {
    local dongles="$1"
    local sched="$2"
    local logfile="/tmp/codexion_extra_dongles_${dongles}_${sched}.log"

    timeout 20 "$BINARY" 10 1500 100 100 100 "$dongles" 20 "$sched" \
        > "$logfile" 2>&1

    local status=$?

    if [ $status -eq 124 ]; then
        fail "dongles=$dongles ($sched): timeout/deadlock"
        return
    fi

    if [ $status -ne 0 ]; then
        fail "dongles=$dongles ($sched): exit status $status"
        return
    fi

    if [ ! -s "$logfile" ]; then
        fail "dongles=$dongles ($sched): empty log"
        return
    fi

    if ! grep -qE "$LOG_REGEX" "$logfile"; then
        fail "dongles=$dongles ($sched): malformed/no valid log"
        return
    fi

    pass "dongles=$dongles ($sched): completed"
}

run_dongle_case 1 fifo
run_dongle_case 1 edf
run_dongle_case 2 fifo
run_dongle_case 2 edf

# ---------------------------------------------------------------------------
section "5. Timing boundary tests"
# ---------------------------------------------------------------------------
# Test values around the evaluation-sheet timing boundary instead of only
# generous values.

for timing in 60 61 100; do
    logfile="/tmp/codexion_extra_timing_${timing}.log"

    timeout 15 "$BINARY" 3 1000 "$timing" "$timing" "$timing" 3 20 fifo \
        > "$logfile" 2>&1

    status=$?

    if [ $status -eq 124 ]; then
        fail "timing=$timing: timeout"
        continue
    fi

    if [ $status -ne 0 ]; then
        fail "timing=$timing: exit status $status"
        continue
    fi

    if [ ! -s "$logfile" ]; then
        fail "timing=$timing: empty log"
        continue
    fi

    pass "timing=$timing: completed"
done

# ---------------------------------------------------------------------------
section "6. Zero/near-zero cooldown stress"
# ---------------------------------------------------------------------------
# The original script tests cooldown 0, 50, and 300 only with 2 coders.
# Here we combine very small cooldowns with more contention.

for cooldown in 0 1 5; do
    logfile="/tmp/codexion_extra_cooldown_${cooldown}.log"

    timeout 20 "$BINARY" 20 1500 100 100 100 1 "$cooldown" fifo \
        > "$logfile" 2>&1

    status=$?

    if [ $status -eq 124 ]; then
        fail "cooldown=$cooldown under contention: timeout"
        continue
    fi

    if [ $status -ne 0 ]; then
        fail "cooldown=$cooldown under contention: exit status $status"
        continue
    fi

    if [ ! -s "$logfile" ]; then
        fail "cooldown=$cooldown under contention: empty log"
        continue
    fi

    pass "cooldown=$cooldown under contention: completed"
done

# ---------------------------------------------------------------------------
section "7. Argument boundary / numeric robustness"
# ---------------------------------------------------------------------------
# These are additional parser cases not present in the original script.

run_expect_fail() {
    local desc="$1"
    shift

    "$BINARY" "$@" > /tmp/codexion_extra_args.log 2>&1
    local status=$?

    if [ $status -eq 0 ]; then
        fail "$desc (accepted invalid input)"
    else
        pass "$desc (rejected, exit $status)"
    fi
}

run_expect_fail "decimal coder count" \
    2.5 800 200 100 100 3 50 fifo

run_expect_fail "decimal burnout time" \
    2 800.5 200 100 100 3 50 fifo

run_expect_fail "negative compile time" \
    2 800 -1 100 100 3 50 fifo

run_expect_fail "negative debug time" \
    2 800 200 -1 100 3 50 fifo

run_expect_fail "negative refactor time" \
    2 800 200 100 -1 3 50 fifo

run_expect_fail "negative cooldown" \
    2 800 200 100 100 3 -1 fifo

run_expect_fail "empty scheduler" \
    2 800 200 100 100 3 50 ""

run_expect_fail "letters mixed with number" \
    2 800ms 200 100 100 3 50 fifo

# ---------------------------------------------------------------------------
section "8. Immediate-burnout / shutdown stress"
# ---------------------------------------------------------------------------
# This deliberately gives coders very little time to survive.
# The important property here is clean termination rather than successful work.

run_shutdown_case() {
    local coders="$1"
    local sched="$2"
    local logfile="/tmp/codexion_extra_shutdown_${coders}_${sched}.log"

    timeout 15 "$BINARY" "$coders" 60 100 100 100 1 5 "$sched" \
        > "$logfile" 2>&1

    local status=$?

    if [ $status -eq 124 ]; then
        fail "shutdown ($coders, $sched): timed out"
        return
    fi

    if [ $status -ne 0 ]; then
        fail "shutdown ($coders, $sched): exit status $status"
        return
    fi

    if ! grep -q "burned out" "$logfile"; then
        fail "shutdown ($coders, $sched): no burnout occurred"
        return
    fi

    # After the final burnout line, coders may finish an in-progress
    # phase (debugging/refactoring) that they already started before
    # burnout — that's expected. Only NEW work (acquiring dongles or
    # starting a fresh compile) after burnout is an actual violation.
    local last_burnout_line
    last_burnout_line=$(grep -n "burned out" "$logfile" | tail -1 | cut -d: -f1)

    local new_work_after
    new_work_after=$(tail -n +"$((last_burnout_line + 1))" "$logfile" | grep -E "has taken a dongle|is compiling")

    if [ -n "$new_work_after" ]; then
        fail "shutdown ($coders, $sched): NEW WORK started after final burnout"
        echo "$new_work_after" | head -5 | sed 's/^/    /'
        return
    fi

    pass "shutdown ($coders, $sched): clean burnout (in-progress phases allowed to finish)"
}

run_shutdown_case 2 fifo
run_shutdown_case 2 edf
run_shutdown_case 10 fifo
run_shutdown_case 10 edf

# ---------------------------------------------------------------------------
section "9. Repeated high-contention race stress"
# ---------------------------------------------------------------------------
# Short repeated runs are useful because scheduling races are nondeterministic.
# A program can pass once and still deadlock occasionally.

RACE_RUNS=20
RACE_FAILS=0

for i in $(seq 1 "$RACE_RUNS"); do
    logfile="/tmp/codexion_extra_race_${i}.log"

    timeout 10 "$BINARY" 20 700 100 100 100 1 1 fifo \
        > "$logfile" 2>&1

    status=$?

    if [ $status -eq 124 ]; then
        RACE_FAILS=$((RACE_FAILS + 1))
        red "  run $i: TIMEOUT"
    elif [ $status -ne 0 ]; then
        RACE_FAILS=$((RACE_FAILS + 1))
        red "  run $i: exit status $status"
    elif [ ! -s "$logfile" ]; then
        RACE_FAILS=$((RACE_FAILS + 1))
        red "  run $i: EMPTY OUTPUT"
    else
        printf "  run %d/%d: OK\n" "$i" "$RACE_RUNS"
    fi
done

if [ $RACE_FAILS -eq 0 ]; then
    pass "race stress: $RACE_RUNS/$RACE_RUNS completed"
else
    fail "race stress: $RACE_FAILS/$RACE_RUNS failed"
fi

# ---------------------------------------------------------------------------
section "10. FIFO vs EDF output comparison"
# ---------------------------------------------------------------------------
# This does NOT automatically declare one scheduler correct.
# It simply produces two logs and tells you whether their acquisition
# sequences differ.
#
# A difference is useful evidence that the scheduler is actually influencing
# contention. Identical output is NOT automatically a failure because timing
# can legitimately make the sequences coincide.

FIFO_LOG="/tmp/codexion_extra_compare_fifo.log"
EDF_LOG="/tmp/codexion_extra_compare_edf.log"

timeout 20 "$BINARY" 10 2000 100 100 100 1 20 fifo > "$FIFO_LOG" 2>&1
FIFO_STATUS=$?

timeout 20 "$BINARY" 10 2000 100 100 100 1 20 edf > "$EDF_LOG" 2>&1
EDF_STATUS=$?

if [ $FIFO_STATUS -ne 0 ] || [ $EDF_STATUS -ne 0 ]; then
    fail "FIFO vs EDF comparison: one scheduler failed"
else
    FIFO_TAKES=$(grep "has taken a dongle" "$FIFO_LOG")
    EDF_TAKES=$(grep "has taken a dongle" "$EDF_LOG")

    if [ "$FIFO_TAKES" != "$EDF_TAKES" ]; then
        pass "FIFO vs EDF: acquisition sequences differ"
    else
        yellow "  NOTE: FIFO and EDF produced the same acquisition sequence this run"
        yellow "        This is not automatically a failure."
        yellow "        Logs saved as:"
        yellow "        $FIFO_LOG"
        yellow "        $EDF_LOG"
    fi
fi

# ---------------------------------------------------------------------------
section "Summary"
# ---------------------------------------------------------------------------

echo ""
green "Passed: $PASS"

if [ $FAIL -gt 0 ]; then
    red "Failed: $FAIL"
    echo ""
    yellow "Some failures may be timing-dependent. Re-run failed stress tests."
    exit 1
else
    green "Failed: 0"
    echo ""
    green "All extra automated checks passed."
    echo ""
    yellow "Still inspect manually:"
    echo "  - $FIFO_LOG"
    echo "  - $EDF_LOG"
    echo "  - FIFO vs EDF ordering against the project specification"
    echo "  - dongle ownership/order under contention"
    echo "  - Norminette"
    exit 0
fi
