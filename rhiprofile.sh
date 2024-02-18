BIN='/tmp/rhiprofile_14ac6ad51b0a48fbbd1f8a878fd2f104'
SRC=rhiprofile.cpp
CPP=g++
OPTS=(-std=c++20 -Ofast)
INCLUDES=(
	-I"$UTILS/rhi/src"
	$(pkg-config --cflags glib-2.0)
)
LIBS=(
	-L"$UTILS/rhi/libs"
	-Wl,-rpath="$UTILS/rhi/libs"
	-lrhi
	-lxxhash
	-lglib-2.0
	-lcryptopp
)

rm -rf "$BIN"
if [ "$1" == -d ]; then
	"$CPP" -Og -g "$SRC" -o "$BIN" "${OPTS[@]}" "${INCLUDES[@]}" "${LIBS[@]}"
	valgrind \
		--tool=memcheck \
		--track-origins=yes "$BIN"
else
	"$CPP" "$SRC" -o "$BIN" "${OPTS[@]}" "${INCLUDES[@]}" "${LIBS[@]}"
	"$BIN"
fi
