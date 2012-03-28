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
  'data' => 
  array (
    'section' => false,
    'data' => 'I got interpolated.',
  ),
  'expected' => '[
  I got interpolated.
  |data|

  {{data}}
  I got interpolated.
]
',
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
  'desc' => 'Delimiters set outside inverted sections should persist.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\[\s*I\s*got\s*interpolated\.\s*\|data\|\s*\{\{data\}\}\s*I\s*got\s*interpolated\.\s*\]\s*