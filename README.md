# php-mustache

[![GitHub Build Status](https://github.com/jbboehr/php-mustache/workflows/ci/badge.svg)](https://github.com/jbboehr/php-mustache/actions?query=workflow%3Aci)
[![Coverage Status](https://coveralls.io/repos/jbboehr/php-mustache/badge.svg?branch=master&service=github)](https://coveralls.io/github/jbboehr/php-mustache?branch=master)
[![Software License](https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat)](LICENSE.md)

C++ implementation of [Mustache](http://mustache.github.com/) as a PHP extension.

## Installation

### PIE

[PIE](https://github.com/php/pie) requires PHP 8.1 or newer to run, but it can
install the extension for PHP 8.0. Install libmustache 0.6.0 or newer, then run:

```sh
pie install jbboehr/php-mustache
```

If libmustache is installed under a non-standard prefix, pass it explicitly:

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

Windows source builds require the PHP SDK, Visual Studio 2022, CMake, and a
static libmustache build. The [Windows CI script](.github/scripts/windows.ps1)
contains the setup currently used to build the extension. Pre-built PIE DLLs
are not currently published, so PIE installation is not available on Windows.

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

See also: [template loader example](examples.md)

## License

The MIT License (MIT). Please see [License File](LICENSE.md) for more information.
