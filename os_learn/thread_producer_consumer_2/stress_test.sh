#!/usr/bin/env bash
set -eu

RUNS="${1:-80}"
TIMEOUT_SEC="${TIMEOUT_SEC:-4}"
BIN="${BIN:-./exe}"
LOG_FILE="${LOG_FILE:-/tmp/pc_stress_last.log}"

if [ ! -x "$BIN" ]; then
  make --no-print-directory >/dev/null
fi

echo "Stress testing: runs=$RUNS timeout=${TIMEOUT_SEC}s bin=$BIN"

i=1
while [ "$i" -le "$RUNS" ]; do
  set +e
  timeout "${TIMEOUT_SEC}s" "$BIN" >"$LOG_FILE" 2>&1
  rc=$?
  set -e

  if rg -n "POTENTIAL DEADLOCK WARNING" "$LOG_FILE" >/dev/null 2>&1; then
    echo "HIT: deadlock warning on run $i"
    echo "log: $LOG_FILE"
    tail -n 40 "$LOG_FILE"
    exit 0
  fi

  if [ "$rc" -eq 124 ]; then
    echo "TIMEOUT on run $i (no warning string found)"
  fi

  i=$((i + 1))
done

echo "NO WARNING in $RUNS runs"
echo "last log: $LOG_FILE"
