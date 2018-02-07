--TEST--
GH-46 HTML inside tag
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$mustache = new Mustache();
echo $mustache->render('a{{ <html> }}b', array());
?>
--EXPECT--
ab
