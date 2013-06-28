--TEST--
MustacheTemplate::setFromBinary() member function
--SKIPIF--
<?php 
if( !extension_loaded('mustache') ) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$tmpl = new MustacheTemplate('{{#block}} {{test}} {{/block}}');
$r = $m->compile($tmpl);
$binstr = $tmpl->toBinary();
var_dump(bin2hex($binstr));
$tmpl2 = new MustacheTemplate();
$tmpl2->setFromBinary($binstr);
var_dump(bin2hex($tmpl2->toBinary()));
var_dump($m->render($tmpl2, array('test' => 'bar', 'block' => true)));
?>
--EXPECT--
string(210) "4d5500010000000000010000005b4d55004000000006000400000047626c6f636b004d5500020000000200000000000020004d5500100100000500000000000074657374004d5500020000000200000000000020004d55008000000006000000000000626c6f636b00"
string(210) "4d5500010000000000010000005b4d55004000000006000400000047626c6f636b004d5500020000000200000000000020004d5500100100000500000000000074657374004d5500020000000200000000000020004d55008000000006000000000000626c6f636b00"
string(5) " bar "