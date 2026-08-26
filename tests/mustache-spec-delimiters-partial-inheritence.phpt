--TEST--
Partial Inheritence
--DESCRIPTION--
Delimiters set in a parent template should not affect a partial.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Partial Inheritence',
  'desc' => 'Delimiters set in a parent template should not affect a partial.',
  'data' => 
  array (
    'value' => 'yes',
  ),
  'partials' => 
  array (
    'include' => '.{{value}}.',
  ),
  'template' => '[ {{>include}} ]
{{= | | =}}
[ |>include| ]
',
  'expected' => '[ .yes. ]
[ .yes. ]
',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"], $test["partials"]), "</render>";
?>
--EXPECT--
<render>[ .yes. ]
[ .yes. ]
</render>