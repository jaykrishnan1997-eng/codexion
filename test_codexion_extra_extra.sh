#!/bin/bash
#
# test_codexion_more.sh
#
# Additional targeted tests for Codexion, covering things not exercised by
# test_codexion.sh or test_codexion_extra.sh:
#   - exact compile-count verification
#   - EDF tie-breaking behavior (relevant to the documented recode scenario)
#   - required_compiles = 1 edge case
#   - cooldown >> burnout adversarial case
#   - max coder count from the eval sheet ceiling
#
# Run from the project root:
#     chmod +x test_codexion_more.sh
#     ./test_codexion_more.sh
#

BINARY="./codexion"

PASS=0
FAIL=0

green()  { printf '\033[32m%s\033[0m\n' "$1"; }
red()    { printf '\033[31m%s\033[0m\n' "$1"; }
yellow() { printf '\033[33m%s\033[0m\n' "$1"; }

pass() { PASS=$((PASS + 1)); green "  PASS: $1"; }
fail() { FAIL=$((FAIL + 1)); red   "  FAIL: $1"; }

section() { printf '\n\033[1m== %s ==\033[0m\n' "$1"; }

# ---------------------------------------------------------------------------
section "1. Exact compile-count verification"
# ---------------------------------------------------------------------------
# Confirms every coder compiles EXACTLY required_compiles times, not more,
# not fewer, when the simulation ends via the "all done" path (no burnout).

check_compile_counts() {
    local coders="$1" required="$2" sched="$3"
    local logfile="/tmp/codexion_more_counts_${coders}_${sched}.log"

    timeout 20 "$BINARY" "$coders" 5000 200 100 100 "$required" 50 "$sched" \
        > "$logfile" 2>&1
    local status=$?

    if [ $status -ne 0 ]; then
        fail "compile counts ($coders coders, req=$required): non-zero exit ($status)"
        return
    fi

    if grep -q "burned out" "$logfile"; then
        fail "compile counts ($coders coders, req=$required): unexpected burnout, can't verify counts"
        return
    fi

    local bad=0
    for ((c = 1; c <= coders; c++)); do
        local count
        count=$(grep -cE "^[0-9]+ $c is compiling$" "$logfile")
        if [ "$count" -lt "$required" ]; then
            red "    coder $c compiled $count times (expected at least $required)"
            bad=1
        fi
    done

    if [ "$bad" -eq 0 ]; then
        pass "compile counts ($coders coders, req=$required, $sched): every coder compiled at least $required times"
    else
        fail "compile counts ($coders coders, req=$required, $sched): some coder fell short"
    fi
}

check_compile_counts 3 4 fifo
check_compile_counts 5 2 edf
check_compile_counts 10 3 fifo

# ---------------------------------------------------------------------------
section "2. required_compiles = 1 (minimal stop condition)"
# ---------------------------------------------------------------------------

run_min_required() {
    local sched="$1"
    local logfile="/tmp/codexion_more_minreq_${sched}.log"

    timeout 15 "$BINARY" 3 3000 200 100 100 1 50 "$sched" > "$logfile" 2>&1
    local status=$?

    if [ $status -ne 0 ]; then
        fail "required_compiles=1 ($sched): non-zero exit ($status)"
        return
    fi

    if grep -q "burned out" "$logfile"; then
        fail "required_compiles=1 ($sched): unexpected burnout"
        return
    fi

    local bad=0
    for c in 1 2 3; do
        count=$(grep -cE "^[0-9]+ $c is compiling$" "$logfile")
        if [ "$count" -lt 1 ]; then
            bad=1
        fi
    done

    if [ "$bad" -eq 0 ]; then
        pass "required_compiles=1 ($sched): stopped correctly after minimal target"
    else
        fail "required_compiles=1 ($sched): some coder never compiled"
    fi
}

run_min_required fifo
run_min_required edf

# ---------------------------------------------------------------------------
section "3. Cooldown >> burnout (adversarial, should reliably burn out cleanly)"
# ---------------------------------------------------------------------------
# When cooldown vastly exceeds burnout time, a coder cannot possibly get a
# dongle back in time. This should burn out promptly, not hang.

run_adversarial_cooldown() {
    local sched="$1"
    local logfile="/tmp/codexion_more_adversarial_${sched}.log"

    timeout 15 "$BINARY" 2 500 200 100 100 3 2000 "$sched" > "$logfile" 2>&1
    local status=$?

    if [ $status -eq 124 ]; then
        fail "cooldown>>burnout ($sched): timed out (should burn out, not hang)"
        return
    fi

    if [ $status -ne 0 ]; then
        fail "cooldown>>burnout ($sched): unexpected exit status $status"
        return
    fi

    if ! grep -q "burned out" "$logfile"; then
        fail "cooldown>>burnout ($sched): expected a burnout, none occurred"
        return
    fi

    pass "cooldown>>burnout ($sched): burned out cleanly as expected"
}

run_adversarial_cooldown fifo
run_adversarial_cooldown edf

# ---------------------------------------------------------------------------
section "4. Maximum coder count ceiling (per eval sheet: up to 200)"
# ---------------------------------------------------------------------------

run_max_coders() {
    local sched="$1"
    local logfile="/tmp/codexion_more_maxcoders_${sched}.log"

    timeout 60 "$BINARY" 200 6000 200 100 100 2 50 "$sched" > "$logfile" 2>&1
    local status=$?

    if [ $status -eq 124 ]; then
        fail "200 coders ($sched): timed out"
        return
    fi

    if [ $status -ne 0 ]; then
        fail "200 coders ($sched): unexpected exit status $status"
        return
    fi

    if [ ! -s "$logfile" ]; then
        fail "200 coders ($sched): empty log"
        return
    fi

    pass "200 coders ($sched): completed without crashing"
}

run_max_coders fifo
run_max_coders edf

# ---------------------------------------------------------------------------
section "5. EDF tie-breaking behavior (informational — see recode note below)"
# ---------------------------------------------------------------------------
# This does not assert a specific winner: it surfaces cases where several
# coders' deadlines could plausibly tie (e.g. all starting near t=0 with the
# same time_to_burnout), so you can manually inspect acquisition order and
# compare it against whatever tie-break rule you implement.
#
# NOTE: the evaluation sheet documents a specific recode scenario:
#   "Modify EDF to prefer higher coder_id on equal deadlines. Validate via
#   contention tests."
# This section does not verify that rule (since your current code may not
# implement it yet) — it just gives you a reproducible contention scenario
# to test your tie-break implementation against once you add it.

logfile="/tmp/codexion_more_edf_tie.log"
timeout 15 "$BINARY" 6 3000 200 100 100 2 50 edf > "$logfile" 2>&1
status=$?

if [ $status -ne 0 ]; then
    fail "EDF tie scenario: non-zero exit ($status)"
else
    yellow "  EDF contention log saved to $logfile"
    yellow "  Inspect the FIRST batch of \"has taken a dongle\" lines (all coders"
    yellow "  start with equal deadlines) to see your current tie-break order."
    yellow "  If you implement the documented recode (prefer higher coder_id on"
    yellow "  equal deadlines), re-run this and confirm the order matches."
    pass "EDF tie scenario: ran cleanly (manual inspection required)"
fi

# ---------------------------------------------------------------------------
section "Summary"
# ---------------------------------------------------------------------------

echo ""
green "Passed: $PASS"

if [ $FAIL -gt 0 ]; then
    red "Failed: $FAIL"
    exit 1
else
    green "Failed: 0"
    echo ""
    yellow "Remember: the eval sheet names a specific recode scenario —"
    yellow "  'Modify EDF to prefer higher coder_id on equal deadlines.'"
    yellow "Make sure you can perform this change live and re-validate with"
    yellow "section 5 above before your defense."
    exit 0
fi