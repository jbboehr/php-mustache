--TEST--
Mustache::render() handles lambda parameter arities and remains reusable after arity errors
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$mustache = new Mustache();
$data = array(
  'name' => 'Ada',
  'zero' => function () {
    return 'zero';
  },
  'one' => function ($text) {
    return strlen($text) . ':' . bin2hex($text);
  },
  'two' => function ($text, MustacheLambdaHelper $helper) {
    return $helper->render('{{name}}') . ':' . $text;
  },
  'optional' => function ($text, MustacheLambdaHelper $helper, $value = 'default') {
    return $value;
  },
  'required' => function ($text, MustacheLambdaHelper $helper, $value) {
    return 'unreachable';
  },
);

var_dump($mustache->render('{{#zero}}ignored{{/zero}}', $data));
var_dump($mustache->render("{{#one}}a\0b{{/one}}", $data));
var_dump($mustache->render('{{#two}}raw{{/two}}', $data));
var_dump($mustache->render('{{#optional}}ignored{{/optional}}', $data));

try {
  $mustache->render('{{#required}}ignored{{/required}}', $data);
  echo "required callback did not throw\n";
} catch (ArgumentCountError $e) {
  echo get_class($e), "\n";
}

var_dump($mustache->render('{{name}}', $data));
?>
--EXPECT--
string(4) "zero"
string(8) "3:610062"
string(7) "Ada:raw"
string(7) "default"
ArgumentCountError
string(3) "Ada"
