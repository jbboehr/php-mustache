--TEST--
Mustache::render() member function - Accepts MustacheTemplate
--SKIPIF--
<?php 

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$tmpl = '{{>test1}}{{>test2}}';
$data = new MustacheData(array(
  'var' => 'val',
));
$partials = array(
  'test1' => '{{var}}',
	'test2' => $m->compile('{{var}}'),
);
$r = $m->render($tmpl, $data, $partials);
var_dump($r);
?>
--EXPECT--
string(6) "valval"