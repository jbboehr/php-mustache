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
  'expected' => '"Phil" == "Phil"',
  'template' => '"{{#a}}{{b.c.d.e.name}}{{/a}}" == "Phil"',
  'desc' => 'The first part of a dotted name should resolve as any other name.',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
"Phil"\s+\=\=\s+"Phil"