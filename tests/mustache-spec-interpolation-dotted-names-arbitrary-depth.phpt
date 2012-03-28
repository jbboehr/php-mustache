--TEST--
Dotted Names - Arbitrary Depth
--DESCRIPTION--
Dotted names should be functional to any level of nesting.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Dotted Names - Arbitrary Depth',
  'data' => 
  array (
    'a' => 
    array (
      'b' => 
      array (
        'c' => 
        array (
          'd' => 
          array (
            'e' => 
            array (
              'name' => 'Phil',
            ),
          ),
        ),
      ),
    ),
  ),
  'expected' => '"Phil" == "Phil"',
  'template' => '"{{a.b.c.d.e.name}}" == "Phil"',
  'desc' => 'Dotted names should be functional to any level of nesting.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"Phil"\s*\=\=\s*"Phil"