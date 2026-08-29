--TEST--
Variable Name Collision
--DESCRIPTION--
Comments must never render, even if variable with same name exists.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Variable Name Collision',
  'desc' => 'Comments must never render, even if variable with same name exists.',
  'data' => 
  array (
    '! comment' => 1,
    '! comment ' => 2,
    '!comment' => 3,
    'comment' => 4,
  ),
  'template' => 'comments never show: >{{! comment }}<',
  'expected' => 'comments never show: ><',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>comments never show: ><</render>
