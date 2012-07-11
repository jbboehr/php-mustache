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
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\[\s*I\s*got\s*interpolated\.\s*\|data\|\s*\{\{data\}\}\s*I\s*got\s*interpolated\.\s*\]\s*