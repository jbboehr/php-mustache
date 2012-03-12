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
  'data' => 
  array (
    'value' => 'yes',
  ),
  'expected' => '[ .yes. ]
[ .yes. ]
',
  'template' => '[ {{>include}} ]
{{= | | =}}
[ |>include| ]
',
  'desc' => 'Delimiters set in a parent template should not affect a partial.',
  'partials' => 
  array (
    'include' => '.{{value}}.',
  ),
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\[\s*\.yes\.\s*\]\s*\[\s*\.yes\.\s*\]\s*