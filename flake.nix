{
  description = "Cobra Audio Engine";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in {
        devShells.default = pkgs.mkShell {
          name = "clion-dev";

          nativeBuildInputs = with pkgs; [
            cmake
            ninja
            gcc
            rustc
            cargo
            gdb
            pkg-config
            git
          ];

          buildInputs = [
          ];

          shellHook = ''
            echo "env is ready, let's go build <3"
          '';
        };
      });
}
