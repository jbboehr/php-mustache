--TEST--
MustacheData copies backed property values without invoking getter hooks
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
if (PHP_VERSION_ID < 80400) die('skip property hooks require PHP 8.4');
?>
--FILE--
<?php
class HookedPropertyContext
{
    public static int $getCalls = 0;

    public string $name = 'Ada' {
        get {
            ++self::$getCalls;
            return strtoupper($this->name);
        }
        set => trim($value);
    }

    public string $displayName {
        get {
            ++self::$getCalls;
            return 'Countess';
        }
    }

    public string $uninitialized {
        get {
            ++self::$getCalls;
            return $this->uninitialized ?? 'fallback';
        }
    }
}

$context = new HookedPropertyContext();
$mustache = new Mustache();
$template = 'name={{name}}|display={{displayName}}|uninitialized={{uninitialized}}';
echo 'source: ', $mustache->render($template, $context), "\n";
echo 'AST: ', $mustache->render($mustache->parse($template), $context), "\n";

$data = new MustacheData($context);
$values = $data->toValue();
echo 'converted name: ', $values['name'], "\n";
echo 'virtual present: ';
var_dump(array_key_exists('displayName', $values));
echo 'uninitialized present: ';
var_dump(array_key_exists('uninitialized', $values));
echo 'converted render: ', $mustache->render($template, $data), "\n";
echo 'get calls after conversion: ', HookedPropertyContext::$getCalls, "\n";

echo 'PHP reads: ', $context->name, '|', $context->displayName, '|', $context->uninitialized, "\n";
echo 'get calls after PHP reads: ', HookedPropertyContext::$getCalls, "\n";

$context->name = ' Lin ';
echo 'snapshot: ', $mustache->render('{{name}}', $data), "\n";
echo 'fresh: ', $mustache->render('{{name}}', $context), "\n";
echo 'get calls after update: ', HookedPropertyContext::$getCalls, "\n";
?>
--EXPECT--
source: name=Ada|display=|uninitialized=
AST: name=Ada|display=|uninitialized=
converted name: Ada
virtual present: bool(false)
uninitialized present: bool(false)
converted render: name=Ada|display=|uninitialized=
get calls after conversion: 0
PHP reads: ADA|Countess|fallback
get calls after PHP reads: 3
snapshot: Ada
fresh: Lin
get calls after update: 3
