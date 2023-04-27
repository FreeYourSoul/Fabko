{
  description = "Flakes for the Fabko multi-agent library";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/master";
  inputs.flake-utils.url = "github:numtide/flake-utils";
  inputs.devshell.url = "github:numtide/devshell/master";

  outputs = { self, nixpkgs, flake-utils }: {

    flake-utils.lib.eachDefaultSystem (system:

      let pkgs = nixpkgs.legacyPackages.${system}; in
      rec {
        packages = flake-utils.lib.flattenTree {
          hello = pkgs.hello;
          gitAndTools = pkgs.gitAndTools;
        };
        defaultPackage = packages.hello;
        apps.hello = flake-utils.lib.mkApp { drv = packages.hello; };
        defaultApp = apps.hello;
      }

  };
}
