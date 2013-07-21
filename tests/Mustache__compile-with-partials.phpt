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
string(234) "000000080000002400000030000000370000003b0000004100000061000000690000006f02002201292110021303160003017465737400030220000203130416000204220730181b34180c1305181b2719001c1819261305211b180f1a1a21160002052206292116000306626172000307666f6f00"