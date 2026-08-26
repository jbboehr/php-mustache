--TEST--
Post-Partial Behavior
--DESCRIPTION--
Delimiters set in a partial should not affect the parent template.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Post-Partial Behavior',
  'desc' => 'Delimiters set in a partial should not affect the parent template.',
  'data' => 
  array (
    'value' => 'yes',
  ),
  'partials' => 
  array (
    'include' => '.{{value}}. {{= | | =}} .|value|.',
  ),
  'template' => '[ {{>include}} ]
[ .{{value}}.  .|value|. ]
',
  'expected' => '[ .yes.  .yes. ]
[ .yes.  .|value|. ]
',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"], $test["partials"]), "</render>";
?>
--EXPECT--
<render>[ .yes.  .yes. ]
[ .yes.  .|value|. ]
</render>