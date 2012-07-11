--TEST--
Dotted Names - Initial Resolution
--DESCRIPTION--
The first part of a dotted name should resolve as any other name.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Dotted Names - Initial Resolution',
  'desc' => 'The first part of a dotted name should resolve as any other name.',
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
    'b' => 
    array (
      'c' => 
      array (
        'd' => 
        array (
          'e' => 
          array (
            'name' => 'Wrong',
          ),
        ),
      ),
    ),
  ),
  'template' => '"{{#a}}{{b.c.d.e.name}}{{/a}}" == "Phil"',
  'expected' => '"Phil" == "Phil"',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"Phil"\s*\=\=\s*"Phil"