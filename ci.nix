let
    generateMustacheDrv = { pkgs, php, phpYaml }:
        let
            mustache_spec = pkgs.callPackage (import ((fetchTarball {
                url = https://github.com/jbboehr/mustache-spec/archive/5b85c1b58309e241a6f7c09fa57bd1c7b16fa9be.tar.gz;
                sha256 = "1h9zsnj4h8qdnzji5l9f9zmdy1nyxnf8by9869plyn7qlk71gdyv";
            }))) {};
        in
        pkgs.runCommand "pecl-mustache.tgz" {
            buildInputs = [ php ];
            src = builtins.filterSource
                (path: type: baseNameOf path != ".idea" && baseNameOf path != ".git" && baseNameOf path != "ci.nix")
                ./.;
        } ''
            cp -r $src/* .
            chmod -R +w tests
            php -d extension=${phpYaml}/lib/php/extensions/yaml.so generate-tests.php ${mustache_spec}/share/mustache-spec/specs
            pecl package | tee tmp.txt
            pecl_tgz=$(cat tmp.txt | grep -v Warning | awk '{print $2}')
            echo $pecl_tgz
            cp $pecl_tgz $out
        '';

    generateMustacheTestsForPlatform = { pkgs, php, buildPecl, phpMustacheSrc }:
        pkgs.recurseIntoAttrs {
            mustache = pkgs.callPackage ./default.nix {
               inherit php buildPecl phpMustacheSrc;
            };
        };
in
builtins.mapAttrs (k: _v:
  let
    path = builtins.fetchTarball {
       url = https://github.com/NixOS/nixpkgs/archive/release-19.09.tar.gz;
       name = "nixpkgs-19.09";
    };
    pkgs = import (path) { system = k; };

    phpMustacheSrc = generateMustacheDrv {
        inherit pkgs;
        inherit (pkgs) php;
        phpYaml = pkgs.phpPackages.yaml;
    };
  in
  pkgs.recurseIntoAttrs {
    peclDist = phpMustacheSrc;

    php71 = let
        path = builtins.fetchTarball {
           url = https://github.com/NixOS/nixpkgs/archive/release-19.03.tar.gz;
           name = "nixpkgs-19.03";
        };
        pkgs = import (path) { system = k; };
        php = pkgs.php71;
    in generateMustacheTestsForPlatform {
        inherit pkgs php phpMustacheSrc;
        buildPecl = pkgs.callPackage "${path}/pkgs/build-support/build-pecl.nix" { inherit php; };
    };

    php72 = let
        php = pkgs.php72;
    in generateMustacheTestsForPlatform {
        inherit pkgs php phpMustacheSrc;
        buildPecl = pkgs.callPackage "${path}/pkgs/build-support/build-pecl.nix" { inherit php; };
    };

    php73 = let
        php = pkgs.php73;
    in generateMustacheTestsForPlatform {
        inherit pkgs php phpMustacheSrc;
        buildPecl = pkgs.callPackage "${path}/pkgs/build-support/build-pecl.nix" { inherit php; };
    };

    php74 = let
        path = builtins.fetchTarball {
           url = https://github.com/NixOS/nixpkgs/archive/master.tar.gz;
           name = "nixpkgs-unstable";
        };
        pkgs = import (path) { system = k; };
        php = pkgs.php74;
    in generateMustacheTestsForPlatform {
        inherit pkgs php phpMustacheSrc;
        buildPecl = pkgs.callPackage "${path}/pkgs/build-support/build-pecl.nix" { inherit php; };
    };

    php = let
        path = builtins.fetchTarball {
           url = https://github.com/NixOS/nixpkgs/archive/master.tar.gz;
           name = "nixpkgs-unstable";
        };
        pkgs = import (path) { system = k; };
        php = pkgs.php;
    in generateMustacheTestsForPlatform {
        inherit pkgs php phpMustacheSrc;
        buildPecl = pkgs.callPackage "${path}/pkgs/build-support/build-pecl.nix" { inherit php; };
    };
  }
) {
  x86_64-linux = {};
  # Uncomment to test build on macOS too
  # x86_64-darwin = {};
}
