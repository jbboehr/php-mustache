--TEST--
MustacheLambdaHelper rejects use after its lambda callback returns
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$mustache = new Mustache();
$retained = null;
try {
  (new ReflectionClass(MustacheLambdaHelper::class))->newInstanceWithoutConstructor();
  echo "constructor bypass accepted\n";
} catch (ReflectionException $e) {
  echo "constructor bypass rejected\n";
}

$data = array(
  'value' => 'safe',
  'capture' => function ($text, MustacheLambdaHelper $helper) use (&$retained) {
    $retained = $helper;
    return $helper->render($text);
  },
);

var_dump($mustache->render('{{#capture}}{{value}}{{/capture}}', $data));
try {
  $retained->render('{{value}}');
  echo "retained helper remained active\n";
} catch (MustacheException $e) {
  var_dump($e->getMessage());
}

$retained = null;
$ast = $mustache->parse('{{#capture}}{{value}}{{/capture}}');
var_dump($mustache->render($ast, $data));
try {
  $retained->render('{{value}}');
  echo "retained AST helper remained active\n";
} catch (MustacheException $e) {
  var_dump($e->getMessage());
}

$retained = null;
try {
  $mustache->render('{{#fail}}x{{/fail}}', array(
    'fail' => function ($text, MustacheLambdaHelper $helper) use (&$retained) {
      $retained = $helper;
      throw new RuntimeException('callback failed');
    },
  ));
} catch (RuntimeException $e) {
  var_dump($e->getMessage());
}
try {
  $retained->render('{{value}}');
  echo "exceptional helper remained active\n";
} catch (MustacheException $e) {
  var_dump($e->getMessage());
}
?>
--EXPECT--
constructor bypass rejected
string(4) "safe"
string(41) "Lambda render context is no longer active"
string(4) "safe"
string(41) "Lambda render context is no longer active"
string(15) "callback failed"
string(41) "Lambda render context is no longer active"
