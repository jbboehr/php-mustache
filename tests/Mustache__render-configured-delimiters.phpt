--TEST--
Mustache::render() compiles templates and partials with configured delimiters
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$mustache = new Mustache();
$mustache->setStartSequence('<%');
$mustache->setStopSequence('%>');

var_dump($mustache->render(
    '<%value%> <%>partial%>',
    array('value' => 'main'),
    array('partial' => '<%value%>')
));
?>
--EXPECT--
string(9) "main main"
