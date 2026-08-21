--TEST--
MustacheData rejects reinitialization during render
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
set_error_handler(static function ($severity, $message) {
  echo $message, "\n";
});

$data = new MustacheData(['stable' => true]);
$data->__construct(['replacement' => true]);
var_dump($data->toValue());

$mustache = new Mustache();
$data = null;
$callback = function () use (&$data) {
  $data->__construct(['more' => 'changed']);
  return '';
};
$data = new MustacheData([
  'callback' => $callback,
  'more' => 'stable',
]);
var_dump($mustache->render('{{#callback}}unused{{/callback}}{{more}}', $data));

restore_error_handler();
$data = null;
unset($callback);
gc_collect_cycles();
?>
--EXPECT--
MustacheData::__construct(): MustacheData is already initialized
array(1) {
  ["stable"]=>
  bool(true)
}
MustacheData::__construct(): MustacheData is already initialized
string(6) "stable"
