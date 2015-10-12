--TEST--
MustacheLambdaHelper::render() member function - will render using existing data with closure
--SKIPIF--
<?php

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();

$tmpl = <<<EOL
{{#one}}placeholder{{/one}}{{#outer}}{{#two}}placeholder{{/two}}{{/outer}}
EOL;

$data = array(
  'one' => function ($text, \MustacheLambdaHelper $helper) {
    return $helper->render('{{outer.inner}}');
  },
  'two' => function ($text, \MustacheLambdaHelper $helper) {
    return $helper->render('{{inner}}');
  },
  'outer' => array(
    'inner' => 'value',
  ),
);
$r = $m->render($tmpl, $data);
var_dump($r);
?>
--EXPECT--
string(10) "valuevalue"