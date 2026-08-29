--TEST--
Mustache::render() member function - will not crash if data contains array with circular reference
--SKIPIF--
<?php

if(!extension_loaded('mustache')) die('skip ');
 ?>
--FILE--
<?php
$m = new Mustache();
$data = array();
$data['var'] = &$data;
set_error_handler(function ($severity, $message) {
  echo "warning:", $message, "\n";
  return true;
});
try {
  $m->render('{{var}}', $data);
} catch (ValueError) {
  echo "ValueError\n";
}
restore_error_handler();
unset($data);
gc_collect_cycles();
echo "collected\n";
?>
--EXPECT--
ValueError
collected
