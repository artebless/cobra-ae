clear
chmod +x build_static_lib.sh
./build_static_lib.sh
mkdir flex/lib
cp build/libcobra_ae.a flex/lib/libcobra_ae.a
# shellcheck disable=SC2164
cd flex
cargo build --release -v
echo '[ RUST FFI COMPILED ]'
