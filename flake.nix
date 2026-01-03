{
  description = "CLion Environment for Cobra Audio Engine";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    rust-overlay.url = "github:oxalica/rust-overlay";
    flake-utils = {
      url = "github:numtide/flake-utils";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, rust-overlay, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [ (import rust-overlay) ];
        };

        rustToolchain = pkgs.rust-bin.stable."1.88.0".default.override {
          extensions = [ "rust-src" "clippy" "rustfmt" ];
        };
      in {
        devShell = pkgs.mkShell {
          nativeBuildInputs = with pkgs; [
            rustToolchain
          ];

          buildInputs = with pkgs; [
            openssl
            pkg-config
            cmake
            git
            ninja
            gcc
            gdb
            rust-analyzer

            # jetbrains.rust-rover
            # jetbrains.clion
            # Clion already installed in system-wide environment.
            # So, because of that I don't need to use another CLion or Rust Rover instance.
            # But if you need, you can uncomment and use one of these IDE's in your environment.

          ];

          shellHook = ''
            mkdir -p ~/.rust-rover/toolchain

            ln -sfn ${rustToolchain}/lib ~/.rust-rover/toolchain
            ln -sfn ${rustToolchain}/bin ~/.rust-rover/toolchain

            export RUST_SRC_PATH="$HOME/.rust-rover/toolchain/lib/rustlib/src/rust/library"

            echo '[ Use scripts to build the engine ]'
          '';
        };
      }
    );
}
