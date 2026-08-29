--TEST--
List Contexts
--DESCRIPTION--
All elements on the context stack should be accessible within lists.
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'List Contexts',
  'desc' => 'All elements on the context stack should be accessible within lists.',
  'data' => 
  array (
    'tops' => 
    array (
      0 => 
      array (
        'tname' => 
        array (
          'upper' => 'A',
          'lower' => 'a',
        ),
        'middles' => 
        array (
          0 => 
          array (
            'mname' => '1',
            'bottoms' => 
            array (
              0 => 
              array (
                'bname' => 'x',
              ),
              1 => 
              array (
                'bname' => 'y',
              ),
            ),
          ),
        ),
      ),
    ),
  ),
  'template' => '{{#tops}}{{#middles}}{{tname.lower}}{{mname}}.{{#bottoms}}{{tname.upper}}{{mname}}{{bname}}.{{/bottoms}}{{/middles}}{{/tops}}',
  'expected' => 'a1.A1x.A1y.',
);
$mustache = new Mustache();
echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";
?>
--EXPECT--
<render>a1.A1x.A1y.</render>
