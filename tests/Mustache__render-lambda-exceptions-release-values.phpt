--TEST--
Mustache::render() releases lambda values and arguments after exceptions
--SKIPIF--
<?php
if (!extension_loaded('mustache') || !class_exists('WeakReference')) die('skip ');
?>
--FILE--
<?php
function reportException(callable $callback) {
  try {
    $callback();
    echo "callback did not throw\n";
  } catch (RuntimeException $e) {
    var_dump($e->getMessage());
  }
}

$mustache = new Mustache();
$weakHelper = null;
$closure = function ($text, MustacheLambdaHelper $helper) use (&$weakHelper) {
  $weakHelper = WeakReference::create($helper);
  throw new RuntimeException('closure failed');
};
$weakClosure = WeakReference::create($closure);
$data = array('fail' => $closure);
unset($closure);
reportException(function () use ($mustache, $data) {
  $mustache->render('{{#fail}}x{{/fail}}', $data);
});
gc_collect_cycles();
var_dump($weakHelper->get());
unset($data);
gc_collect_cycles();
var_dump($weakClosure->get());

class InvokableFailure {
  public function __invoke($text) {
    throw new RuntimeException('invokable failed');
  }
}

$invokable = new InvokableFailure();
$weakInvokable = WeakReference::create($invokable);
$data = array('fail' => $invokable);
unset($invokable);
reportException(function () use ($mustache, $data) {
  $mustache->render('{{#fail}}x{{/fail}}', $data);
});
unset($data);
gc_collect_cycles();
var_dump($weakInvokable->get());

class MethodFailure {
  public function fail($text) {
    throw new RuntimeException('method failed');
  }
}

$object = new MethodFailure();
$weakObject = WeakReference::create($object);
reportException(function () use ($mustache, $object) {
  $mustache->render('{{#fail}}x{{/fail}}', $object);
});
unset($object);
gc_collect_cycles();
var_dump($weakObject->get());

class ThrowingString {
  public static $destructions = 0;

  public function __toString() {
    throw new RuntimeException('string conversion failed');
  }

  public function __destruct() {
    self::$destructions++;
  }
}

reportException(function () use ($mustache) {
  $mustache->render('{{value}}', array(
    'value' => function () {
      return new ThrowingString();
    },
  ));
});
gc_collect_cycles();
var_dump(ThrowingString::$destructions);
?>
--EXPECT--
string(14) "closure failed"
NULL
NULL
string(16) "invokable failed"
NULL
string(13) "method failed"
NULL
string(24) "string conversion failed"
int(1)
