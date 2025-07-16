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
, lcov
, gcovr
, execute_test ? false
, with_coverage ? false
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
  ] ++ lib.optionals execute_test [
    lcov gcovr
  ];


  CXXFLAGS = lib.optionalString with_coverage "-coverage -fkeep-inline-functions -fno-inline -fno-inline-small-functions -fno-default-inline -O0 -g";

  postCheck = lib.optionalString with_coverage ''
      echo "Generating coverage report... ${src}"

      mkdir -p $out/coverage

      # Generate gcovr Cobertura XML report
      gcovr --root . \
            --filter=".*\.(cpp|hpp|hh)$" \
            --exclude="/nix/store/.*" \
            --gcov-executable=gcov \
            --exclude-unreachable-branches \
            --xml \
            --xml-pretty \
            --output=$out/coverage/cobertura.xml \

  '';

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
    description = "Fabko Agent - compiler library";
    license = [ unfree ];
    platforms = platforms.all;
  };
}
