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
  'data' => 
  array (
    'value' => 'yes',
  ),
  'expected' => '[ .yes.  .yes. ]
[ .yes.  .|value|. ]
',
  'template' => '[ {{>include}} ]
[ .{{value}}.  .|value|. ]
',
  'desc' => 'Delimiters set in a partial should not affect the parent template.',
  'partials' => 
  array (
    'include' => '.{{value}}. {{= | | =}} .|value|.',
  ),
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"], $test["partials"]);
?>
--EXPECTREGEX--
\[\s*\.yes\.\s*\.yes\.\s*\]\s*\[\s*\.yes\.\s*\.\|value\|\.\s*\]\s*