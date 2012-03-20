Mustache
========

C++ implementation of Mustache as a PHP extension. See <http://mustache.github.com/>

Features
--------

All features of Mustache are supported EXCEPT:

* Lambda functions
* Whitespace rules. All whitespace is kept as it is in the input template.

Requirements
------------

Ubuntu:

```bash
apt-get install git-core php5-dev
```

Installation
------------

Ubuntu:

```bash
git clone git://github.com/jbboehr/php-mustache.git
cd php-mustache
git submodule init
git submodule update
phpize
./configure --enable-mustache
make && make install
```

Usage
-----

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
  echo $mustache->render($tmpl, $data);
}
```

Produces:

```text
valvalvalvalval
```

Todo
-----

* Fix whitespace non-conformity
* Cache compiled templates in memory
* Fix std::bad_alloc when xdebug is enabled

Tested On
-----

* Ubuntu 11.10 / GCC 4.6.1 / PHP 5.3.6-13ubuntu3.6
* Ubuntu 10.04 LTS / GCC 4.4.3 / PHP 5.3.2
* FreeBSD 9.0 / GCC 4.2.1 / PHP 5.3.8
