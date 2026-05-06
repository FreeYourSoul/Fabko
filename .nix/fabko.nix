{ lib
, stdenv
, cmake
, ninja
, gcc
, catch2_3
, fmt
, rocksdb
, spdlog
, nlohmann_json
, liburing
, lcov
, gcovr
, fil
, execute_test ? false
, with_coverage ? false
}:

let
    version = builtins.readFile ../VERSION;
in
stdenv.mkDerivation rec {
  pname = "fabko";
  inherit version;

  src = ../.;

  nativeBuildInputs = [
    cmake ninja gcc
  ] ++ lib.optionals execute_test [
    lcov gcovr
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

  cmakeFlags = [
    "-DBUILD_TESTING=${if execute_test || with_coverage then "ON" else "OFF"}"
    "-DCMAKE_PREFIX_PATH=${fil}/lib/cmake"
    "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
    "-DCMAKE_INSTALL_PREFIX=${placeholder "out"}"
    "-DCMAKE_INSTALL_INCLUDEDIR=${placeholder "out"}/include"
    "-DCMAKE_INSTALL_LIBDIR=${placeholder "out"}/lib"
  ];

  # change compilation flags when doing code coverage execution
  # remove optimization and disable inlining
  CXXFLAGS = lib.optionalString with_coverage "-coverage -fkeep-inline-functions -fno-inline -fno-inline-small-functions -fno-default-inline -O0 -g";

  # execute unit tests only if the tests are required to be executed
  doCheck = execute_test;

  postCheck = lib.optionalString with_coverage ''
    echo "Generating coverage report..."

    mkdir -p $out/coverage

    # Copy compile_commands.json to $out if it exists
    if [ -f compile_commands.json ]; then
      cp compile_commands.json $out/
    fi

    if sourceDir=$(grep "^CMAKE_HOME_DIRECTORY:INTERNAL=" CMakeCache.txt 2>/dev/null | cut -d= -f2) \
       && [ -n "$sourceDir" ]; then
      echo "Coverage: source directory from CMakeCache.txt: $sourceDir"
    else
      sourceDir="$(pwd)/.."
      echo "Coverage: CMakeCache.txt lookup failed, using fallback: $sourceDir"
    fi

    # --root "$sourceDir"             : path gcovr uses to resolve source-file paths
    #                                   embedded in .gcno files; also makes output paths
    #                                   repo-relative (e.g. include/fil/algorithm/contains.hh)
    #                                   which is required for Codacy to match them.
    # --object-directory "."          : search the CMake build dir (CWD in postCheck)
    #                                   for .gcda/.gcno files; required because --root is
    #                                   now the source dir, not the build dir.
    # --exclude "$sourceDir/tests/.*" : keep test drivers out of the library report.
    gcovr \
          --root "$sourceDir" \
          --object-directory "." \
          --exclude "$sourceDir/tests/.*" \
          --gcov-executable gcov \
          --exclude-unreachable-branches \
          --xml \
          --xml-pretty \
          --output "$out/coverage/cobertura.xml"

  '';

  postInstall = "";

  meta = with lib; {
    description = "Fabko Agent - library to create and make agent that communicate through a DSL (fabl)";
    homepage = "https://github.com/FreeYourSoul/Fabko";
    platforms = platforms.all;
    maintainers = with maintainers; [
      freeyoursoul
    ];
  };
}
