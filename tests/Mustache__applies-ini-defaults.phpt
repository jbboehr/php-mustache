--TEST--
Mustache applies configured INI defaults
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--INI--
mustache.default_escape=0
mustache.default_start="<%"
mustache.default_stop="%>"
--FILE--
<?php
var_dump(ini_get('mustache.default_escape'));
var_dump(ini_get('mustache.default_start'));
var_dump(ini_get('mustache.default_stop'));

$mustache = new Mustache();
var_dump($mustache->getEscapeByDefault());
var_dump($mustache->getStartSequence());
var_dump($mustache->getStopSequence());
var_dump($mustache->render('<%value%>', array('value' => '<configured>')));

ini_set('mustache.default_escape', '1');
ini_set('mustache.default_start', '[[');
ini_set('mustache.default_stop', ']]');
$runtimeMustache = new Mustache();
var_dump($runtimeMustache->getEscapeByDefault());
var_dump($runtimeMustache->getStartSequence());
var_dump($runtimeMustache->getStopSequence());
var_dump($runtimeMustache->render('[[value]]', array('value' => '<runtime>')));
?>
--EXPECT--
string(1) "0"
string(2) "<%"
string(2) "%>"
bool(false)
string(2) "<%"
string(2) "%>"
string(12) "<configured>"
bool(true)
string(2) "[["
string(2) "]]"
string(15) "&lt;runtime&gt;"
