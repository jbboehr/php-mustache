--TEST--
MustacheData retains and releases objects and closures used as lambdas
--SKIPIF--
<?php
if (!extension_loaded('mustache') || !class_exists('WeakReference')) die('skip ');
?>
--FILE--
<?php
class MethodLambda {
  public $data;

  public function value() {
    return 'method';
  }

  public function other() {
    return 'other';
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

$object = new MethodLambda();
$objectData = new MustacheData($object);
$object->data = $objectData;
$weakObject = WeakReference::create($object);
$weakObjectData = WeakReference::create($objectData);
unset($object, $objectData);
gc_collect_cycles();
var_dump($weakObject->get());
var_dump($weakObjectData->get());

class ClosureCycleHolder {
  public $data;
}

$holder = new ClosureCycleHolder();
$closure = static function () use ($holder) {
  return 'closure';
};
$closureData = new MustacheData(array(array($closure)));
$holder->data = $closureData;
$weakHolder = WeakReference::create($holder);
$weakClosure = WeakReference::create($closure);
$weakClosureData = WeakReference::create($closureData);
unset($holder, $closure, $closureData);
gc_collect_cycles();
var_dump($weakHolder->get());
var_dump($weakClosure->get());
var_dump($weakClosureData->get());
?>
--EXPECT--
bool(true)
NULL
bool(true)
NULL
NULL
NULL
NULL
NULL
NULL
