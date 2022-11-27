{
  description = "php-mustache";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
    libmustache.url = "github:jbboehr/libmustache";
    mustache_spec = {
        url = "github:jbboehr/mustache-spec";
        inputs.mustache_spec.follows = "mustache_spec";
    };
    gitignore = {
        url = "github:hercules-ci/gitignore.nix";
        inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, flake-utils, mustache_spec, libmustache, gitignore }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        php = pkgs.php;
      in
      rec {
        packages = flake-utils.lib.flattenTree rec {
          php-mustache = pkgs.callPackage ./default.nix {
            mustache_spec = mustache_spec.packages.${system}.mustache-spec;
            libmustache = libmustache.packages.${system}.libmustache;
            inherit (gitignore.lib) gitignoreSource;
            buildPecl = pkgs.callPackage (nixpkgs + "/pkgs/build-support/build-pecl.nix") {
                php = php.unwrapped;
            };
            inherit php;
          };
          default = php-mustache;
        };
      }
    );
}
