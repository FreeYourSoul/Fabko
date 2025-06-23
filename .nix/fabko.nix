{ lib
, stdenv
, cmake
, ninja
, fmt
, spdlog
, rocksdb
, catch2_3
, nlohmann_json
, liburing
}:

let
    version = builtins.readFile ../VERSION;
    import-from-json = import ./import-from-json.nix { };
    fil = import-from-json {
      sourcesFile = ./nix-dep.json;
      dependencyName = "fil";
    };
in
stdenv.mkDerivation rec {
  pname = "fabko";
  inherit version;

  src = ../.;

  nativeBuildInputs = [
    cmake ninja
  ];

  buildInputs = [
    fmt
    fil
    spdlog
    rocksdb
    catch2_3
    nlohmann_json
    liburing
  ];

  meta = with lib; {
    description = "Fabko compiler project";
    license = licenses.mit;
    platforms = platforms.all;
  };
}
