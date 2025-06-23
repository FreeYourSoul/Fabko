
{ pkgs ? import <nixpkgs> {} }:

# sourcesFile: path to the JSON file
# dependencyName: name of the dependency in the JSON file
{ sourcesFile, dependencyName }:

let
  sources = builtins.fromJSON (builtins.readFile sourcesFile);
  
  dependencySource = sources.${dependencyName} or (throw "Dependency ${dependencyName} not found in ${sourcesFile}");
  
  src = pkgs.fetchFromGitHub {
    inherit (dependencySource) owner repo rev sha256;
  };
in
  import "${src}/${dependencySource.nixFile or "default.nix"}" { inherit pkgs; }