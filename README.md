Mustache
--------------------------------------------------------------------------------

[![Build Status](https://travis-ci.org/jbboehr/php-mustache.png?branch=master)](https://travis-ci.org/jbboehr/php-mustache)

C++ implementation of Mustache as a PHP extension.
See [Mustache](http://mustache.github.com/)

Features
--------------------------------------------------------------------------------

All features of Mustache are supported EXCEPT:

* Lambda functions
* Whitespace rules. All whitespace is kept as it is in the input template.

Installation
--------------------------------------------------------------------------------

Install [libmustache](https://github.com/jbboehr/libmustache)

Ubuntu:

```bash
sudo apt-get install git-core php5-dev
git clone git://github.com/jbboehr/php-mustache.git --recursive
cd php-mustache
phpize
./configure --enable-mustache
make
sudo make install
echo extension=mustache.so | sudo tee /etc/php5/conf.d/mustache.ini
```

Windows:

* Get a [Mustache Windows Build](https://jbboehr.github.com/php-mustache)
* Place into your PHP extension directory and add `extension=php_mustache.dll` to your php.ini

Note: these pass all of the tests, but may or may not be production safe.


Usage
--------------------------------------------------------------------------------

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

Todo
--------------------------------------------------------------------------------
* Fix whitespace non-conformity
* Cache compiled templates in memory


Compiling on Windows
--------------------------------------------------------------------------------

* Need to add [msinttypes](https://code.google.com/p/msinttypes/) to your include directory.
* See [Build your own PHP on Windows](https://wiki.php.net/internals/windows/stepbystepbuild). 
Note: SetEnv.cmd is not always in the path, mine was in 
`C:\Program Files\Microsoft SDKs\Windows\v6.1\Bin\SetEnv.cmd` and needed to be modified
(modified version is in the win32 folder in this repository)
* Place sources into the ext directory (e.g. `C:\php-sdk\php53dev\vc9\x86\php5.3-xyz\ext\mustache`)
* `configure --enable-mustache=shared ...` or `configure --enable-mustache ...` to compile it into PHP.
  If compiling it as shared, make sure --enable-zts matches the version you have installed.
* `nmake`

To run the tests for the shared extension, add 
`extension=C:\php-sdk\php53\vc9\x86\php5.3-xyz\Release\php_mustache.dll` to 
`C:\php-sdk\php53\vc9\x86\php5.3-xyz\Release\php.ini`
then run `nmake test TESTS=ext\mustache\tests\*` (replace `Release` with `Release_TS` if thread safety is enabled)
