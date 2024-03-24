{ stdenv
, cmake
, pkg-config
, vcpkg
}:

stdenv.mkDerivation {
  pname = "fabko";
  version = "0.1.0";

  src = ./.;

  nativeBuildInputs = [
    cmake
    pkg-config
    vcpkg
  ];

  # Runtime dependencies go here
  buildInputs = [
  ];

  cmakeFlags = [
    "-DCMAKE_TOOLCHAIN_FILE=${vcpkg}/scripts/buildsystems/vcpkg.cmake"
  ];

  meta = with stdenv.lib; {
    description = "Fabko project";
    platforms = platforms.all;
  };
}