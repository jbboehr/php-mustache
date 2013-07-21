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
string(234) "000000080000002400000030000000370000003b0000004100000061000000690000006f020011011810040206030800030174657374000302200002030604080002041107190a1b1d0a0c06050a1b160b000e0a19150605100d0a0f0c0c10080002051106181008000306626172000307666f6f00"