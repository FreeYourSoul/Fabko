{ lib
, stdenv
, cmake
, fmt
, spdlog
, rocksdb
, catch2_3
, nlohmann_json
, liburing
}:

stdenv.mkDerivation rec {
  pname = "fabko";
  version = "0.1.0";

  src = ../.;

  nativeBuildInputs = [
    cmake
  ];

  buildInputs = [
    fmt
    spdlog
    rocksdb
    catch2_3
    nlohmann_json
    liburing
  ];

  cmakeFlags = [
    "-DCMAKE_BUILD_TYPE=Release"
  ];

  meta = with lib; {
    description = "Fabko compiler project";
    license = licenses.mit;
    platforms = platforms.all;
  };
}
