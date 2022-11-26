# php-mustache

[![GitHub Linux Build Status](https://github.com/jbboehr/php-mustache/workflows/linux/badge.svg)](https://github.com/jbboehr/php-mustache/actions?query=workflow%3Alinux)
[![GitHub OSX Build Status](https://github.com/jbboehr/php-mustache/workflows/osx/badge.svg)](https://github.com/jbboehr/php-mustache/actions?query=workflow%3Aosx)
[![GitHub Docker Build Status](https://github.com/jbboehr/php-mustache/workflows/docker/badge.svg)](https://github.com/jbboehr/php-mustache/actions?query=workflow%3Adocker)
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


## Credits

- [John Boehr](https://github.com/jbboehr)
- [Adam Baratz](https://github.com/adambaratz)
- [All Contributors](../../contributors)


## License

The MIT License (MIT). Please see [License File](LICENSE.md) for more information.

## Examples

You can adapt this Mustache_Template_Loader class or use it as an
example of how to interface with whatever framework you use. The MustacheAST
type for example can serialized/unserialized to APCU or other cache.

```
class Mustache_Template_Loader {
  public $partials = [];
  public $partial_pathname_resolver = false;
  public $mustache_instance = false;

  /**
   * Loads and renders a template from a path.
   *
   * @param string   $tmplpath The input template filesystem path.
   * @param mixed    $data     The data argument to Mustache::render.
   * @param callable $callback Resolves a partial name to its filesystem path.
   *
   * @return false|string The string output, or false on failure
   */
  public static function load_and_render($tmplpath, $data, $callback) {
    $loader = new Mustache_Template_Loader();
    $loader->mustache_instance = new \Mustache();
    $loader->partial_pathname_resolver = $callback;
    $template_string = file_get_contents($tmplpath);
    $template_ast = $loader->mustache_instance->parse($template_string);
    $template_ast_array = $template_ast->toArray();
    $loader->resolve_partials($template_ast_array);
    return $loader->mustache_instance->render($template_ast, $data, $loader->partials);
  }

  public function resolve_partials($ast_array) {
    if (($ast_array['type'] ?? null) === 512) {
      // The libmustache src/node.hpp has enum Type TypePartial = 512
      $partial_name = $ast_array['data'];
      $partial_ast = $this->partials[$partial_name] ?? null;
      if ($partial_ast !== null) {
        // this check prevents the performing of extra work
        // and by corollary protects against indefinite recursion.
        return;
      }
      $this->partials[$partial_name] = "";
      $partial_pathname = ($this->partial_pathname_resolver)($partial_name);
      $partial_string = file_get_contents($partial_pathname);
      $partial_ast = $this->mustache_instance->parse($partial_string);
      $this->partials[$partial_name] = $partial_ast;
      $this->resolve_partials($partial_ast->toArray());
    } else {
      // For simplicity the AST node types are ignored here.
      foreach ($ast_array as $ast_array_value) {
        if (is_array($ast_array_value)) {
          $this->resolve_partials($ast_array_value);
        }
      }
    }
  }
}
```
An example use of this class:

```
function template_pathname($partial_name) {
  return '/usr/local/lib/templates/' . $partial_name . '.mustache';
}

echo Mustache_Template_Loader::load_and_render(
       template_pathname('topview'),
       $data,
       'template_pathname'
     );
```
