--TEST--
Mustache partial limits are optional, inclusive, per-instance, and reset per render
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
if (!method_exists(Mustache::class, 'setPartialLimits')) die("missing setPartialLimits\n");

function renderCase($m, $root, $partials): void
{
    try {
        $output = $m->render($root, [], $partials);
        echo 'output=', json_encode($output), "\n";
    } catch (ValueError $e) {
        echo 'rejected ', str_contains($e->getMessage(), 'maxEntries') ? 'maxEntries' : 'maxTextBytes', "\n";
    }
}

$m = new Mustache();
foreach (['source' => '{{>a}}{{>b}}', 'AST' => $m->parse('{{>a}}{{>b}}')] as $backend => $root) {
    echo "$backend\n";
    $m->setPartialLimits();
    renderCase($m, $root, ['a' => 'A', 'b' => 'B']);
    // Two one-byte names and two one-byte sources total four bytes.
    var_dump($m->setPartialLimits(maxEntries: 2, maxTextBytes: 4));
    renderCase($m, $root, ['a' => 'A', 'b' => 'B']);
    renderCase($m, $root, ['a' => 'A', 'b' => 'B']);
    $m->setPartialLimits(maxEntries: 1);
    renderCase($m, $root, ['a' => 'A', 'unused' => '']);
    $m->setPartialLimits(maxTextBytes: 3);
    renderCase($m, $root, ['a' => 'A', 'b' => 'B']);
    renderCase($m, $root, ['a' => 'A']);
    $m->setPartialLimits(0, 0);
    renderCase($m, $root, []);
    renderCase($m, $root, null);
    renderCase($m, $root, ['a' => '']);
    $m->setPartialLimits(maxTextBytes: 0);
    renderCase($m, $root, ['a' => '']);
    renderCase($m, $root, ['' => '']);
    $m->setPartialLimits(null, null);
    renderCase($m, $root, ['a' => 'A', 'b' => 'B']);
}

$m->setPartialLimits(0, 0);
foreach ([[-1, null], [null, -1], [1, -1]] as $invalid) {
    try {
        $m->setPartialLimits(...$invalid);
        echo "invalid accepted\n";
    } catch (ValueError $e) {
        echo "invalid rejected\n";
    }
    // A failed setter must leave both previous limits intact.
    renderCase($m, '{{>a}}', ['a' => 'A']);
}
renderCase(new Mustache(), '{{>a}}', ['a' => 'A']);
$m->setPartialLimits(PHP_INT_MAX, PHP_INT_MAX);
renderCase($m, '{{>a}}', ['a' => 'A']);

// Default state must also exist when a subclass omits the parent constructor.
class PartialLimitSubclass extends Mustache { public function __construct() {} }
renderCase(new PartialLimitSubclass(), '{{>a}}', ['a' => 'A']);
?>
--EXPECT--
source
output="AB"
NULL
output="AB"
output="AB"
rejected maxEntries
rejected maxTextBytes
output="A"
output=""
output=""
rejected maxEntries
rejected maxTextBytes
output=""
output="AB"
AST
output="AB"
NULL
output="AB"
output="AB"
rejected maxEntries
rejected maxTextBytes
output="A"
output=""
output=""
rejected maxEntries
rejected maxTextBytes
output=""
output="AB"
invalid rejected
rejected maxEntries
invalid rejected
rejected maxEntries
invalid rejected
rejected maxEntries
output="A"
output="A"
output="A"
