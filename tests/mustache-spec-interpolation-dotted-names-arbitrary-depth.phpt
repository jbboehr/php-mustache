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
  'desc' => 'Dotted names should be functional to any level of nesting.',
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
  'template' => '"{{a.b.c.d.e.name}}" == "Phil"',
  'expected' => '"Phil" == "Phil"',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"Phil"\s*\=\=\s*"Phil"