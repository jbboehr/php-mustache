# php-mustache

[![Software License](https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat)](LICENSE.md)
[![Build Status](https://travis-ci.org/jbboehr/php-mustache.png?branch=master)](https://travis-ci.org/jbboehr/php-mustache)
[![Coverage Status](https://coveralls.io/repos/jbboehr/php-mustache/badge.svg?branch=master&service=github)](https://coveralls.io/github/jbboehr/php-mustache?branch=master)

C++ implementation of [Mustache](http://mustache.github.com/) as a PHP extension.


## Features

All features of Mustache are supported EXCEPT:

* Whitespace rules. All whitespace is kept as it is in the input template.


## Installation

#### Linux

For Ubuntu LTS, the extension is available in a [PPA](https://launchpad.net/~jbboehr/+archive/ubuntu/mustache), or via source:

Install [libmustache](https://github.com/jbboehr/libmustache)

``` sh
sudo apt-get install git-core php5-dev
git clone git://github.com/jbboehr/php-mustache.git --recursive
cd php-mustache
phpize
./configure --enable-mustache
make
sudo make install
echo extension=mustache.so | sudo tee /etc/php5/conf.d/mustache.ini
```

#### OSX

You can install the PHP Mustache extension & the libmustache dependancy on OSX using [Homebrew](http://brew.sh/)

``` sh
brew install php56-mustache
```

Homebrew has versions of Mustache available for PHP 5.3 - 5.6, just replace `56` above with the version you want; `libmustache` is a dependancy of the extension so you don't need to install it separately.

#### Windows

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

Example 2:

```php
<?php
$mustache = new Mustache();
$tmpl = $mustache->compile('{{var}}');
$data = new MustacheData(array('var' => 'val'));
for( $i = 0; $i < 5; $i++ ) {
  echo $mustache->execute($tmpl, $data);
}
```

Produces:

```text
valvalvalvalval
```


## Credits

- [John Boehr](https://github.com/jbboehr)
- [Adam Baratz](https://github.com/adambaratz)
- [All Contributors](../../contributors)


## License

The MIT License (MIT). Please see [License File](LICENSE.md) for more information.

