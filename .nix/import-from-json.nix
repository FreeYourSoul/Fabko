
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

  # Import the dependency's nix file
  imported = import "${src}/${dependencySource.nixFile or "default.nix"}" { inherit pkgs; };

in
  # Ensure we return a derivation
  if pkgs.lib.isDerivation imported then
    imported.overrideAttrs (oldAttrs: {
      # Force CMAKE_INSTALL_PREFIX to be set correctly
      cmakeFlags = (oldAttrs.cmakeFlags or []) ++ [
        "-DCMAKE_INSTALL_PREFIX=${placeholder "out"}"
        "-DCMAKE_INSTALL_LIBDIR=lib"
        "-DCMAKE_INSTALL_INCLUDEDIR=include"
      ];

      # Ensure install phase runs
      doInstallCheck = true;
      postInstall = ''
        echo "-- import-from-json :: POST INSTALL: Fixing CMake config files for ${dependencyName}: new path ${imported} --"

        # Find and fix CMake config files that contain temporary build paths
        find $out/lib/cmake -name "*.cmake" -type f -exec \
        sed -i 's|/build/[^/]*[^/]*/[^;]*||g' {} \;

        # Also fix any references to the temporary source directory
        find $out/lib/cmake -name "*.cmake" -type f -exec \
        sed -i 's|/build/[^;]*-source/||g' {} \;

        # Replace temporary paths with relative paths to the installation directory
        find $out/lib/cmake -name "*.cmake" -type f -exec \
        sed -i "s|/build/[^;]*|\${imported}}/../../..|g" {} \;
      '';



    })
  else
    throw "Expected derivation from ${dependencyName}, got: ${builtins.typeOf imported}"
