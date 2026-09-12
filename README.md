# php-mustache

[![GitHub Build Status](https://github.com/jbboehr/php-mustache/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/jbboehr/php-mustache/actions/workflows/ci.yml)
[![Coverage Status](https://coveralls.io/repos/jbboehr/php-mustache/badge.svg?branch=master&service=github)](https://coveralls.io/github/jbboehr/php-mustache?branch=master)
[![Software License](https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat)](LICENSE.md)
[![AI burn](https://img.shields.io/endpoint?url=https%3A%2F%2Fgist.githubusercontent.com%2Fjbboehr%2F01576ea7c08e2401b6121904462024a3%2Fraw%2Fagent-badge.json&cacheSeconds=300)](https://github.com/arlegotin/agent-badge)

C++ implementation of [Mustache](http://mustache.github.com/) as a PHP extension.

Upgrading from 0.9.x? Read the [migration guide](docs/upgrading.md) before updating.

## Installation

### PIE

[PIE](https://github.com/php/pie) requires PHP 8.1 or newer to run, but it can
install the extension for PHP 8.0:

```sh
pie install jbboehr/php-mustache
```

For releases with matching binary assets, PIE downloads the extension with
libmustache linked statically. These packages target Windows x64 with PHP
8.0–8.5 and macOS 15 or newer on Apple Silicon with PHP 8.2–8.5, in both
thread-safe and non-thread-safe modes. Check the
[release assets](https://github.com/jbboehr/php-mustache/releases) for availability.

On Windows, PIE requires a matching binary; it cannot compile the extension.
Other Windows architectures and PHP builds are unsupported by these packages.

On Unix systems, including macOS, PIE builds from source when no matching
binary is available. Install the [source requirements](#source), including
libmustache 0.6.0 or newer. For source builds with libmustache under a non-standard
prefix, pass it explicitly. This option selects a source build on Unix even when
a matching binary is available:

```sh
pie install jbboehr/php-mustache --with-libmustache=/path/to/prefix
```

### Source

Requirements:

* PHP 8.0 or newer, including the development headers and tools
* A C++17 compiler
* GNU Make, Automake, Autoconf, and Libtool
* `pkg-config`
* [`libmustache`](https://github.com/jbboehr/libmustache) 0.6.0 or newer

```sh
git clone https://github.com/jbboehr/php-mustache.git
cd php-mustache
phpize
./configure --enable-mustache
make
make test
sudo make install
```

If libmustache is installed under a non-standard prefix, pass
`--with-libmustache=/path/to/prefix` to `configure`.

Add the extension to `php.ini`:

```ini
extension=mustache.so
```

### Nix

Use the flake package to build a PHP environment with the extension enabled:

```nix
{
  inputs.php-mustache.url = "github:jbboehr/php-mustache";

  outputs = {php-mustache, ...}: let
    system = "x86_64-linux";
    mustache = php-mustache.packages.${system}.default;
  in {
    packages.${system}.default = mustache.php.buildEnv {
      extensions = ({enabled, ...}: enabled ++ [mustache]);
    };
  };
}
```

The default package uses PHP 8.3. Replace `default` with a matrix package such
as `php85-gcc` to select another PHP version.

### Windows

PIE installation requires a matching DLL ZIP in the selected release; it cannot
fall back to compiling on Windows. The binary includes libmustache, so no
separate libmustache installation is needed.

For source builds, install the PHP SDK and matching Visual C++ toolset. Download
a [static libmustache SDK](https://github.com/jbboehr/libmustache/blob/master/docs/windows-binaries.md)
matching PHP's architecture and toolset, and pass its extracted directory as
`--with-libmustache=C:\path\to\libmustache`. Use v142 for PHP 8.0–8.3 and v143
for PHP 8.4–8.5. Both TS and NTS builds use the SDK's `/MD` runtime variant.

## Usage

Example:

```php
<?php
$mustache = new Mustache();
$tmpl = <<<EOF
Hello {{name}}
You have just won {{value}} dollars!
{{#in_ca}}
Well, {{taxed_value}} dollars, after taxes.
{{/in_ca}}
EOF;
$data = array(
  'name' => 'John',
  'value' => 10000,
  'taxed_value' => 10000 * 0.6,
  'in_ca' => true,
);
$partials = array();
echo $mustache->render($tmpl, $data, $partials);
```

Produces:

```text
Hello John
You have just won 10000 dollars!
Well, 6000 dollars, after taxes.
```

See the [official Mustache manual](https://mustache.github.io/mustache.5.html)
for template syntax, the [PHP API guide](docs/php-api.md) for data conversion,
lambdas, errors, and limits, and the [template loader example](examples.md)
for loading templates from files.

`MustacheData` and `MustacheLambdaHelper` are final classes and cannot be
subclassed. To add application behavior around converted data, store a
`MustacheData` instance in your own class and pass the stored `MustacheData`
instance to `Mustache::render()`. The extension supplies `MustacheLambdaHelper`
to section lambdas; use the helper only during its callback.

PHP also rejects `ReflectionClass::newInstanceWithoutConstructor()` for these
two classes.

`Mustache`, `MustacheTemplate`, and `MustacheAST` remain extensible. The
[PHP stub](mustache.stub.php) describes their public methods and types.

## License

The MIT License (MIT). Please see [License File](LICENSE.md) for more information.
