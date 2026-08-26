--TEST--
Inverted Sections
--DESCRIPTION--
Delimiters set outside inverted sections should persist.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Inverted Sections',
  'desc' => 'Delimiters set outside inverted sections should persist.',
  'data' => 
  array (
    'section' => false,
    'data' => 'I got interpolated.',
  ),
  'template' => '[
{{^section}}
  {{data}}
  |data|
{{/section}}

{{= | | =}}
|^section|
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