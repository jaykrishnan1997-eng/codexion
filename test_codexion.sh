#!/bin/bash
#
# test_codexion.sh
#
# Test script for the Codexion project, built around the criteria on the
# official evaluation sheet (compilation, global variables, easy/less-easy/
# medium test tiers, log format, leaks). Run from the project root (the
# directory containing your Makefile).
#
# Usage: ./test_codexion.sh
#

BINARY="./codexion"
LOG_REGEX='^[0-9]+ [0-9]+ (has taken a dongle|is compiling|is debugging|is refactoring|burned out)$'

PASS=0
FAIL=0

green() { printf '\033[32m%s\033[0m\n' "$1"; }
red()   { printf '\033[31m%s\033[0m\n' "$1"; }
yellow(){ printf '\033[33m%s\033[0m\n' "$1"; }

pass() { PASS=$((PASS+1)); green "  PASS: $1"; }
fail() { FAIL=$((FAIL+1)); red   "  FAIL: $1"; }

section() { printf '\n\033[1m== %s ==\033[0m\n' "$1"; }

# ---------------------------------------------------------------------------
section "1. Compilation"
# ---------------------------------------------------------------------------

if [ ! -f Makefile ]; then
    red "No Makefile found in current directory. Aborting."
    exit 1
fi

make fclean > /dev/null 2>&1

COMPILE_OUTPUT=$(make 2>&1)
COMPILE_STATUS=$?

if [ $COMPILE_STATUS -ne 0 ]; then
    fail "make did not succeed"
    echo "$COMPILE_OUTPUT"
    exit 1
else
    pass "make succeeded"
fi

if echo "$COMPILE_OUTPUT" | grep -qi "warning"; then
    fail "compilation produced warnings (must be zero with -Wall -Wextra -Werror)"
    echo "$COMPILE_OUTPUT" | grep -i "warning"
else
    pass "no compilation warnings"
fi

if [ ! -f "$BINARY" ]; then
    fail "binary '$BINARY' not found (must be named exactly 'codexion')"
    exit 1
else
    pass "binary named 'codexion' exists"
fi

# ---------------------------------------------------------------------------
section "2. Global mutable state (heuristic check)"
# ---------------------------------------------------------------------------
# This is a heuristic, not a substitute for manual review: it flags
# top-level (non-static-local) variable definitions in src/*.c that are not
# function prototypes, typedefs, or struct/enum definitions. Manually
# confirm any hits are not shared mutable globals.

SRC_DIR="src"
if [ -d "$SRC_DIR" ]; then
    SUSPECT=$(grep -nE '^\s*(int|long|char|float|double|t_[a-zA-Z_]+)\s+\**[a-zA-Z_][a-zA-Z0-9_]*\s*(=.*)?;' "$SRC_DIR"/*.c 2>/dev/null | grep -v '^\s*static')
    if [ -n "$SUSPECT" ]; then
        yellow "  Possible top-level variable declarations found (verify manually — not all are global state):"
        echo "$SUSPECT" | sed 's/^/    /'
    else
        pass "no obvious top-level global variable declarations found"
    fi
else
    yellow "  src/ directory not found, skipping heuristic scan"
fi

# ---------------------------------------------------------------------------
section "3. Argument validation"
# ---------------------------------------------------------------------------

run_expect_fail() {
    local desc="$1"; shift
    "$BINARY" "$@" > /tmp/codexion_test_out.log 2>&1
    local status=$?
    if [ $status -eq 0 ]; then
        fail "$desc (expected non-zero exit, got 0)"
    else
        pass "$desc (rejected, exit $status)"
    fi
}

run_expect_fail "negative number_of_coders"          -1 800 200 100 100 3 50 fifo
run_expect_fail "non-integer argument"                two 800 200 100 100 3 50 fifo
run_expect_fail "invalid scheduler string"            2 800 200 100 100 3 50 blah
run_expect_fail "missing arguments"                   2 800 200 100 100 3 50
run_expect_fail "zero number_of_coders"               0 800 200 100 100 3 50 fifo
run_expect_fail "extra arguments"                     2 800 200 100 100 3 50 fifo extra

# ---------------------------------------------------------------------------
section "4. Log line format"
# ---------------------------------------------------------------------------

check_log_format() {
    local logfile="$1"
    local bad_lines
    bad_lines=$(grep -vE "$LOG_REGEX" "$logfile")
    if [ -n "$bad_lines" ]; then
        fail "log contains malformed/interleaved lines in $logfile"
        echo "$bad_lines" | head -5 | sed 's/^/    /'
        return 1
    fi
    return 0
}

# ---------------------------------------------------------------------------
section "5. Easy tier — moderate coders, generous timing, no burnout expected"
# ---------------------------------------------------------------------------
# Per the eval sheet: don't test >200 coders, don't test timing under 60ms.

run_easy_case() {
    local coders="$1" sched="$2" burnout="$3"
    local logfile="/tmp/codexion_easy_${coders}_${sched}.log"
    timeout 20 "$BINARY" "$coders" "$burnout" 200 100 100 3 100 "$sched" > "$logfile" 2>&1
    local status=$?
    if [ $status -ne 0 ]; then
        fail "easy case ($coders coders, $sched): non-zero exit ($status)"
        return
    fi
    if grep -q "burned out" "$logfile"; then
        fail "easy case ($coders coders, $sched): unexpected burnout"
        return
    fi
    check_log_format "$logfile" && pass "easy case ($coders coders, $sched): clean run, correct log format"
}

run_easy_case 2 fifo 2000
run_easy_case 2 edf 2000
run_easy_case 5 fifo 2000
run_easy_case 5 edf 2000
run_easy_case 20 fifo 4000
run_easy_case 50 edf 6000

# ---------------------------------------------------------------------------
section "6. Less-easy tier — burnout edge cases"
# ---------------------------------------------------------------------------

run_burnout_case() {
    local desc="$1" coders="$2" burnout="$3" compile="$4" debug="$5" refactor="$6" sched="$7"
    local logfile="/tmp/codexion_burnout_$(date +%s%N).log"
    timeout 15 "$BINARY" "$coders" "$burnout" "$compile" "$debug" "$refactor" 20 60 "$sched" > "$logfile" 2>&1
    local status=$?
    if ! grep -q "burned out" "$logfile"; then
        fail "$desc: expected a burnout, none occurred (exit $status)"
        return
    fi
    # confirm nothing logs after the FIRST burned out line (simulation must stop cleanly)
    local after
    after=$(awk '/burned out/{f=1; next} f' "$logfile" | grep -E "has taken a dongle|is compiling")
    if [ -n "$after" ]; then
        fail "$desc: coder started new work after burnout"
        echo "$after" | head -3 | sed 's/^/    /'
        return
    fi
    check_log_format "$logfile" && pass "$desc: burnout detected, no new work started after"
}

run_burnout_case "tight burnout, 2 coders, fifo"  2 300 200 100 100 fifo
run_burnout_case "tight burnout, 2 coders, edf"   2 300 200 100 100 edf
run_burnout_case "tight burnout, 4 coders, fifo"  4 350 200 100 100 fifo

# ---------------------------------------------------------------------------
section "7. Medium tier — cooldown, scheduler differences, refactor timing"
# ---------------------------------------------------------------------------

run_cooldown_case() {
    local cooldown="$1"
    local logfile="/tmp/codexion_cooldown_${cooldown}.log"
    timeout 15 "$BINARY" 2 2000 200 100 100 3 "$cooldown" fifo > "$logfile" 2>&1
    local status=$?
    if [ $status -ne 0 ]; then
        fail "cooldown=$cooldown: non-zero exit ($status)"
        return
    fi
    check_log_format "$logfile" && pass "cooldown=$cooldown: ran cleanly"
}

run_cooldown_case 0
run_cooldown_case 50
run_cooldown_case 300

# fifo vs edf should both complete without error; a full behavioural diff
# (which coder gets served first under contention) needs manual/log
# inspection - this only confirms both modes run without crashing.
for sched in fifo edf; do
    logfile="/tmp/codexion_sched_${sched}.log"
    timeout 15 "$BINARY" 6 2000 200 100 100 3 60 "$sched" > "$logfile" 2>&1
    status=$?
    if [ $status -ne 0 ]; then
        fail "scheduler=$sched: non-zero exit ($status)"
    else
        check_log_format "$logfile" && pass "scheduler=$sched: ran cleanly (inspect $logfile manually for ordering)"
    fi
done

# ---------------------------------------------------------------------------
section "8. Torture test — repeated runs, watch for hangs/crashes/inconsistency"
# ---------------------------------------------------------------------------

TORTURE_RUNS=30
TORTURE_FAILS=0
for i in $(seq 1 $TORTURE_RUNS); do
    echo "."
    logfile="/tmp/codexion_torture_$i.log"
    timeout 15 "$BINARY" 3 800 200 100 100 3 50 fifo > "$logfile" 2>&1
    status=$?
    if [ $status -ne 0 ] && [ $status -ne 124 ]; then
        # 124 = timeout killed it, which we also treat as a failure below
        :
    fi
    if [ $status -eq 124 ]; then
        TORTURE_FAILS=$((TORTURE_FAILS+1))
        red "  run $i: TIMED OUT (possible hang/deadlock)"
    elif [ $status -ne 0 ]; then
        TORTURE_FAILS=$((TORTURE_FAILS+1))
        red "  run $i: exited with status $status"
    fi
done

if [ $TORTURE_FAILS -eq 0 ]; then
    pass "torture test: $TORTURE_RUNS/$TORTURE_RUNS runs completed cleanly"
else
    fail "torture test: $TORTURE_FAILS/$TORTURE_RUNS runs failed (hang or crash)"
fi

# ---------------------------------------------------------------------------
section "9. Memory leaks (valgrind, if available)"
# ---------------------------------------------------------------------------

if command -v valgrind > /dev/null 2>&1; then
    VG_OUT=$(valgrind --leak-check=full --error-exitcode=42 "$BINARY" 3 2000 200 100 100 3 50 fifo 2>&1 >/dev/null)
    if echo "$VG_OUT" | grep -q "All heap blocks were freed -- no leaks are possible"; then
        pass "valgrind: no leaks detected"
    else
        fail "valgrind: possible leaks or errors detected"
        echo "$VG_OUT" | grep -A 3 "LEAK SUMMARY" | sed 's/^/    /'
    fi
else
    yellow "  valgrind not found, skipping leak check"
fi

# ---------------------------------------------------------------------------
section "10. ThreadSanitizer (optional, if you build a separate -fsanitize=thread binary)"
# ---------------------------------------------------------------------------
yellow "  Not run automatically here (needs a separate build). Suggested manual command:"
yellow "  cc -Wall -Wextra -Werror -pthread -fsanitize=thread -I include src/*.c -o codexion_tsan"
yellow "  ./codexion_tsan 3 800 200 100 100 3 50 fifo"

# ---------------------------------------------------------------------------
section "Summary"
# ---------------------------------------------------------------------------
echo ""
green "Passed: $PASS"
if [ $FAIL -gt 0 ]; then
    red "Failed: $FAIL"
    exit 1
else
    red "Failed: $FAIL"
    green "All automated checks passed. Still manually verify:"
    echo "  - README.md required sections (see eval sheet)"
    echo "  - Norminette / norm compliance"
    echo "  - No dongle duplication under contention (inspect logs by eye)"
    echo "  - EDF vs FIFO actual ordering differences under contention"
    exit 0
fi