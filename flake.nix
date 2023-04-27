{
  description = "Free Agent Board operative is an agent / blackboard library that enable distributed resolution of problem among Agents.";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
    fil.url = "github:FreeYourSoul/FiL";
  };


  outputs = { self, nixpkgs, flake-utils, fil }:
    flake-utils.lib.eachDefaultSystem (system:
    let
        pkgs = nixpkgs.legacyPackages.${system};

    in rec {
         fabko = pkgs.stdenv.mkDerivation {
             pname = "fabko";
             version = "1.0.0";

             src = self;

             buildInputs = [ pkgs.cmake fil.fil pkgs.catch2 pkgs.fmt ];
        };

        defaultPackage = fabko;

    });

}
