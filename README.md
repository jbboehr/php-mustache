# php-mustache

[![GitHub Build Status](https://github.com/jbboehr/php-mustache/workflows/ci/badge.svg)](https://github.com/jbboehr/php-mustache/actions?query=workflow%3Aci)
[![Coverage Status](https://coveralls.io/repos/jbboehr/php-mustache/badge.svg?branch=master&service=github)](https://coveralls.io/github/jbboehr/php-mustache?branch=master)
[![Software License](https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat)](LICENSE.md)

C++ implementation of [Mustache](http://mustache.github.com/) as a PHP extension.


## Features

All features of Mustache are supported EXCEPT:

* Whitespace rules. All whitespace is kept as it is in the input template.


## Installation

### Linux/OSX

#### Source

Prerequisite packages are:

* PHP development headers and tools
* `gcc` >= 4.4 | `clang` >= 3.x | `vc` >= 11
* GNU `make` >= 3.81
* `automake`
* `autoconf`
* [`libmustache`](https://github.com/jbboehr/libmustache)

``` sh
git clone git://github.com/jbboehr/php-mustache.git --recursive
cd php-mustache
phpize
./configure --enable-mustache
make
sudo make install
```

Add the extension to your *php.ini*:

```ini
echo extension=mustache.so | tee -a /path/to/your/php.ini
```

### Fedora/RHEL/CentOS

RPM packages of the extension are available in [Remi's repository](https://rpms.remirepo.net/).

**Fedora** (change 24 to match your Fedora version)

``` sh
dnf install https://rpms.remirepo.net/fedora/remi-release-24.rpm
dnf install --enablerepo=remi php-pecl-mustache
```

**RHEL/CentOS** (for default PHP in base repository)

``` sh
yum install https://rpms.remirepo.net/enterprise/remi-release-7.rpm
yum install php-pecl-mustache
```

### Nix/NixOS

``` sh
nix-env -i -f https://github.com/jbboehr/php-mustache/archive/master.tar.gz
```

### Windows

See [Build your own PHP on Windows](https://wiki.php.net/internals/windows/stepbystepbuild). You may need to add [msinttypes](https://code.google.com/p/msinttypes/) ([export](https://github.com/jbboehr/msinttypes/)) to your include directory.


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

## Credits

- [John Boehr](https://github.com/jbboehr)
- [Adam Baratz](https://github.com/adambaratz)
- [All Contributors](../../contributors)


## License

The MIT License (MIT). Please see [License File](LICENSE.md) for more information.
