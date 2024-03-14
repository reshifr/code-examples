BIN='/tmp/rmvprofile_c49fd855c6688c0e664eac8965f2f4dd'
SRC=rmvprofile.cpp
CPP=g++
OPTS=(-std=c++20 -Ofast)
INCLUDES=(-I"$UTILS/rmv")

rm -rf "$BIN"
rm -rf "$BIN.massif"
if [ "$1" == -d ]; then
	"$CPP" -Og -g "$SRC" -o "$BIN" "${OPTS[@]}" "${INCLUDES[@]}" "${LIBS[@]}"
	valgrind \
		--tool=memcheck \
		--track-origins=yes "$BIN"
elif [ "$1" == -v ]; then
    "$CPP" -Og -g "$SRC" -o "$BIN" "${OPTS[@]}" "${INCLUDES[@]}" "${LIBS[@]}"
    valgrind \
		--tool=massif \
		--time-unit=ms \
		--detailed-freq=1 \
		--max-snapshots=1000 \
		--massif-out-file="$BIN.massif" \
		--log-file=/dev/null "$BIN" > /dev/null
    massif-visualizer "$BIN.massif" &> /dev/null
else
	"$CPP" "$SRC" -o "$BIN" "${OPTS[@]}" "${INCLUDES[@]}" "${LIBS[@]}"
	"$BIN"
fi
