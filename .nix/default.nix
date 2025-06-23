{ pkgs ? import <nixpkgs> {} }:

let
  fabko = pkgs.callPackage ./fabko.nix {  };
in
fabko
