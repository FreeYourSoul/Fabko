{ pkgs ? import <nixpkgs> {} }:

let
  fabko = pkgs.callPackage ./fabko.nix {};
in
pkgs.mkShell {
  buildInputs = with pkgs; [
    # Development tools
    cmake
    pkg-config
    vcpkg
    gcc
    ninja
    gdb
    
    # Include the project's runtime dependencies
  ] ++ fabko.buildInputs;

  # Preserve CPATH, CMAKE_PREFIX_PATH, PKG_CONFIG_PATH etc.
  shellHook = ''
    export CMAKE_PREFIX_PATH=${pkgs.vcpkg}/share/vcpkg
    export VCPKG_ROOT=${pkgs.vcpkg}
  '';

}