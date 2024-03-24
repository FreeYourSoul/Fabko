{ pkgs ? import <nixpkgs> {} }:

pkgs.callPackage ./fabko.nix {
  # You can override dependencies here if needed
}