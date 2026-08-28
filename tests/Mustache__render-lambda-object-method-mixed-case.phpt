--TEST--
Mustache::render() passes section text to mixed-case class method lambdas
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
class Data {
  public function RenderSection($text) {
    return strtoupper($text);
  }
}

$mustache = new Mustache();
var_dump($mustache->render('{{#RenderSection}}value{{/RenderSection}}', new Data()));
?>
--EXPECT--
string(5) "VALUE"
