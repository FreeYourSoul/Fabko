{
pkgs ? import <nixpkgs> {}
, execute_test  ? false
, with_coverage ? false
}:

let
  fabko = pkgs.callPackage ./fabko.nix { inherit execute_test;  inherit with_coverage;  };
in
fabko
