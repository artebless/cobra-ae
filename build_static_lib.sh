clear
cmake -B build cmake -S .
cmake --build build
# shellcheck disable=SC2164
echo '[ STATIC LIBRARY COMPILED ]'