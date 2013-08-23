--TEST--
Mustache::compile() member function - With Partials
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$tmpl = $m->compile('{{test}} {{>foo}}', array(
  'foo' => '{{#foo}}{{bar}}{{/foo}}',
));
var_dump(get_class($tmpl));
var_dump(bin2hex((string) $tmpl));
?>
--EXPECT--
string(12) "MustacheCode"
string(278) "0000000800000024000000340000003c0000004100000049000000730000007d0000008402000011000118100400020600030800030001746573740003000220000200030600040800020004110007190a00241d0a00110600050a0024160b00000e0a002215060005100d0a00150c0c1008000200051100061810080003000662617200030007666f6f00"