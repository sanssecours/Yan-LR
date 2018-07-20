#!/usr/bin/env fish

set parser "Build/badger"
trap cleanup EXIT INT QUIT TERM

function cleanup -d 'Remove temporary files'
    rm -f "$output" "$difference"
end

set IFS (printf '\n\b')
for file in (find Input -depth 1 -type file -name '*.yaml' | sort)
    printf "• Test file “%s”\n" "$file"

    set output (mktemp)
    set -l error_message (eval $parser "\"$file\"" 2>&1 >"$output")
    if test "$status" -ne 0
        printf "\nUnable to parse “%s”:\n\n" "$file" >&2
        printf '%s\n\n' "$error_message" >&2
        set failed 'true'
        continue
    end

    perl -0777pe 's/.*— Output ————\n\n(.*)/\1/sm' -i "$output"
    set difference (mktemp)
    set -l expected (printf "$file" | sed 's/\.[^.]*$/.txt/')
    if ! diff --side-by-side "$output" "$expected" >"$difference"
        printf "\nThe output for “%s” did not match the expected output:\n\n" "$file" >&2
        cat "$difference" >&2
        set failed 'true'
    end
end

if test "$failed" = 'true'
    exit 1
end
