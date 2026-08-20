--TEST--
MustacheData retains and releases objects and closures used as lambdas
--SKIPIF--
<?php
if (!extension_loaded('mustache') || !class_exists('WeakReference')) die('skip ');
?>
--FILE--
<?php
class MethodLambda {
  public function value() {
    return 'method';
  }
}

$object = new MethodLambda();
$weakObject = WeakReference::create($object);
$objectData = new MustacheData($object);
unset($object);
gc_collect_cycles();
var_dump($weakObject->get() instanceof MethodLambda);
unset($objectData);
gc_collect_cycles();
var_dump($weakObject->get());

$closure = static function () {
  return 'closure';
};
$weakClosure = WeakReference::create($closure);
$closureData = new MustacheData($closure);
unset($closure);
gc_collect_cycles();
var_dump($weakClosure->get() instanceof Closure);
unset($closureData);
gc_collect_cycles();
var_dump($weakClosure->get());
?>
--EXPECT--
bool(true)
NULL
bool(true)
NULL
