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
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
\[\s*I\s*got\s*interpolated\.\s*\|data\|\s*\{\{data\}\}\s*I\s*got\s*interpolated\.\s*\]\s*