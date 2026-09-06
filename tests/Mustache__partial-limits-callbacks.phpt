--TEST--
Mustache partial limits preserve getter order and snapshot configuration before callbacks
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
if (!method_exists(Mustache::class, 'setPartialLimits')) die("missing setPartialLimits\n");
class BudgetSource extends MustacheTemplate
{
    public $reads = 0;
    public function __construct(private $source, private $callback = null) { unset($this->template); }
    public function __get($name)
    {
        ++$this->reads;
        if ($this->callback !== null) ($this->callback)();
        return $this->source;
    }
}
function attempt($m, $root, $partials, $data = []): void
{
    try {
        $output = $m->render($root, $data, $partials);
        echo "output=$output\n";
    } catch (ValueError $e) {
        echo "limit rejected\n";
    }
}
$m = new Mustache();
foreach (['source' => '{{>a}}', 'AST' => $m->parse('{{>a}}')] as $backend => $root) {
    echo "$backend\n";
    $a = new BudgetSource('A');
    $b = new BudgetSource('B');
    $c = new BudgetSource('C');
    $m->setPartialLimits(maxEntries: 1);
    attempt($m, $root, ['a' => $a, 'b' => $b]);
    echo "entry reads=$a->reads/$b->reads\n";
    $a->reads = 0;
    $m->setPartialLimits(maxTextBytes: 3);
    attempt($m, $root, ['a' => $a, 'b' => $b, 'c' => $c]);
    echo "text reads=$a->reads/$b->reads/$c->reads\n";

    $m->setPartialLimits();
    $b->reads = 0;
    try {
        $m->render($root, [], ['a' => '{{#missing}}', 'b' => $b]);
    } catch (MustacheParserException $e) {
        echo "parser failure, later reads=$b->reads\n";
    }

    $m->setPartialLimits(1, 2);
    $changing = new BudgetSource('BB', fn() => $m->setPartialLimits());
    attempt($m, $root, ['a' => $changing]);
    echo "changing reads=$changing->reads\n";
    attempt($m, $root, ['a' => 'BB']);

    $m->setPartialLimits(1, 2);
    $nested = new BudgetSource('A', function () use ($m, $root) {
        $m->setPartialLimits(0, 0);
        attempt($m, $root, ['a' => 'A']);
    });
    attempt($m, $root, ['a' => $nested]);
    attempt($m, $root, ['a' => 'A']);
    $m->setPartialLimits();
    attempt($m, $root, ['a' => 'A']);
}

// Capture settings before reading the root wrapper, too.
$m->setPartialLimits(0);
$root = new BudgetSource('{{>a}}', fn() => $m->setPartialLimits());
attempt($m, $root, ['a' => 'A']);
attempt($m, '{{>a}}', ['a' => 'A']);
?>
--EXPECT--
source
limit rejected
entry reads=1/0
limit rejected
text reads=1/1/0
parser failure, later reads=0
limit rejected
changing reads=1
output=BB
limit rejected
output=A
limit rejected
output=A
AST
limit rejected
entry reads=1/0
limit rejected
text reads=1/1/0
parser failure, later reads=0
limit rejected
changing reads=1
output=BB
limit rejected
output=A
limit rejected
output=A
limit rejected
output=A
