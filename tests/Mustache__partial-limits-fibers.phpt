--TEST--
Mustache partial budgets remain local to a suspended render
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
if (PHP_VERSION_ID < 80100) die('skip Fibers require PHP 8.1');
?>
--FILE--
<?php
if (!method_exists(Mustache::class, 'setPartialLimits')) die("missing setPartialLimits\n");
class SuspendedBudgetSource extends MustacheTemplate
{
    public function __construct() { unset($this->template); }
    public function __get($name) { Fiber::suspend('source read'); return 'A'; }
}
$m = new Mustache();
foreach (['source' => '{{>a}}', 'AST' => $m->parse('{{>a}}')] as $backend => $root) {
    $m->setPartialLimits(1, 2);
    $fiber = new Fiber(fn() => $m->render($root, [], ['a' => new SuspendedBudgetSource()]));
    echo "$backend: ", $fiber->start(), "\n";
    $m->setPartialLimits(1, 1);
    try {
        $m->render($root, [], ['a' => 'B']);
        echo "overlap accepted\n";
    } catch (ValueError $e) {
        echo "overlap rejected\n";
    }
    $m->setPartialLimits(0, 0);
    $fiber->resume();
    echo 'resumed: ', $fiber->getReturn(), "\n";
}
?>
--EXPECT--
source: source read
overlap rejected
resumed: A
AST: source read
overlap rejected
resumed: A
