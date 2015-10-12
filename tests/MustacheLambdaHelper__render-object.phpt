--TEST--
MustacheLambdaHelper::render() member function - will render using existing data with object
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

class Data {
  public $outer = array('inner' => 'value');

  function one($text, \MustacheLambdaHelper $helper) {
    return $helper->render('{{outer.inner}}');
  }

  function two($text, \MustacheLambdaHelper $helper) {
    return $helper->render('{{inner}}');
  }
}

$r = $m->render($tmpl, new Data());
var_dump($r);
?>
--EXPECT--
string(10) "valuevalue"