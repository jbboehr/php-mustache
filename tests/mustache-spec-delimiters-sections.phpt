--TEST--
Sections
--DESCRIPTION--
Delimiters set outside sections should persist.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Sections',
  'desc' => 'Delimiters set outside sections should persist.',
  'data' => 
  array (
    'section' => true,
    'data' => 'I got interpolated.',
  ),
  'template' => '[
{{#section}}
  {{data}}
  |data|
{{/section}}

{{= | | =}}
|#section|
  {{data}}
  |data|
|/section|
]
',
  'expected' => '[
  I got interpolated.
  |data|

  {{data}}
  I got interpolated.
]
',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>[
  I got interpolated.
  |data|

  {{data}}
  I got interpolated.
]
</render>