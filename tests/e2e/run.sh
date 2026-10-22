#!/bin/sh
BIN="${1:-./computor}"
DIR="$(dirname "$0")/cases"
pass=0
fail=0

check() {
	label="$1"; got_out="$2"; got_code="$3"; want_out="$4"; want_code="$5"
	if [ "$got_out" = "$want_out" ] && [ "$got_code" = "$want_code" ]; then
		pass=$((pass + 1))
	else
		fail=$((fail + 1))
		echo "KO  $label (code $got_code, attendu $want_code)"
		echo "--- attendu"; echo "$want_out"
		echo "--- obtenu";  echo "$got_out"
	fi
}

for in_file in "$DIR"/*.in; do
	name=$(basename "$in_file" .in)
	eq=$(cat "$in_file")
	want_out=$(cat "$DIR/$name.out")
	want_code=0
	[ -f "$DIR/$name.code" ] && want_code=$(cat "$DIR/$name.code")

	out=$("$BIN" "$eq" 2>/dev/null); code=$?
	check "$name [arg]" "$out" "$code" "$want_out" "$want_code"

	out=$(printf '%s\n' "$eq" | "$BIN" 2>/dev/null); code=$?
	check "$name [stdin]" "$out" "$code" "$want_out" "$want_code"
done

echo "e2e : $pass OK, $fail KO"
[ "$fail" -eq 0 ]
