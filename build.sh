clear
cmake -B build cmake -S .
cmake --build build
mkdir flex/lib
cp build/libcobra_ae.a flex/lib/libcobra_ae.a
# shellcheck disable=SC2164
cd flex
cargo build --release -v
