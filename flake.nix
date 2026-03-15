{
  description = "Fabko: Agent - compiler library";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    fil = {
      url = "github:FreeYourSoul/FiL";
      # The user mentioned FiL has been converted into nix flakes already.
    };
  };

  outputs = { self, nixpkgs, flake-utils, fil }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };

        fabko = pkgs.callPackage ./.nix/fabko.nix {
          fil = fil.packages.${system}.default;
        };

        fabko-test = pkgs.callPackage ./.nix/fabko.nix {
          fil = fil.packages.${system}.default;
          execute_test = true;
        };

        fabko-coverage = pkgs.callPackage ./.nix/fabko.nix {
          fil = fil.packages.${system}.default;
          execute_test = true;
          with_coverage = true;
        };
      in
      {
        packages.default = fabko;
        packages.fabko = fabko;
        packages.fabko-test = fabko-test;
        packages.fabko-coverage = fabko-coverage;

        devShells.default = pkgs.mkShell {
          inputsFrom = [ fabko ];
          nativeBuildInputs = with pkgs; [
            cmake
            ninja
            gcc
            gdb
            lcov
            gcovr
          ];
          shellHook = ''
            echo "Fabko C++ development environment loaded (via Flake)"
          '';
        };
      }
    );
}
