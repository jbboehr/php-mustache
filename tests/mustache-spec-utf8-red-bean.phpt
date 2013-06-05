--TEST--
Red Bean
--DESCRIPTION--
Red Bean
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Red Bean',
  'desc' => 'Red Bean',
  'data' => 
  array (
    'noms' => '紅豆',
    'location' => '南國',
  ),
  'template' => '{{noms}}生{{location}} 春來發幾枝 願君多采擷 此物最相思',
  'expected' => '紅豆生南國 春來發幾枝 願君多采擷 此物最相思',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
紅豆生南國\s*春來發幾枝\s*願君多采擷\s*此物最相思