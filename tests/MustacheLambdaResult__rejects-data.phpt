--TEST--
Explicit result objects are accepted only as callback returns and rejected from ordinary data
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip mustache extension required'); ?>
--FILE--
<?php
if (!class_exists('MustacheLiteralResult') || !class_exists('MustacheTemplateResult')) die("missing explicit lambda results\n");
$m = new Mustache();
foreach ([MustacheLiteralResult::class, MustacheTemplateResult::class] as $class) {
    echo "$class\n";
    $result = new $class('{{name}}');
    $reference =& $result;
    foreach (['root' => fn () => $m->render($result, []),
        'partial' => fn () => $m->render('{{>value}}', [], ['value' => $result])] as $label => $run) {
        try { $run(); echo "$label:accepted\n"; }
        catch (Throwable $e) { echo "$label:", get_class($e), "\n"; }
    }
    foreach (['root' => $result, 'map' => ['value' => $result], 'list' => [$result],
        'property' => (object) ['value' => $result], 'reference' => ['value' => &$reference]] as $label => $data) {
        foreach (['render' => fn () => $m->render('{{value.getText}}', $data),
            'data' => fn () => new MustacheData($data),
            'debug' => fn () => $m->debugDataStructure($data)] as $operation => $run) {
            try { $run(); echo "$label/$operation:accepted\n"; }
            catch (ValueError $e) { echo "$label/$operation:", str_contains($e->getMessage(), 'callback return') ? 'rejected' : $e->getMessage(), "\n"; }
        }
    }
    echo $m->render('{{name}}', ['name' => 'recovered']), "\n";
}
?>
--EXPECT--
MustacheLiteralResult
root:TypeError
partial:ValueError
root/render:rejected
root/data:rejected
root/debug:rejected
map/render:rejected
map/data:rejected
map/debug:rejected
list/render:rejected
list/data:rejected
list/debug:rejected
property/render:rejected
property/data:rejected
property/debug:rejected
reference/render:rejected
reference/data:rejected
reference/debug:rejected
recovered
MustacheTemplateResult
root:TypeError
partial:ValueError
root/render:rejected
root/data:rejected
root/debug:rejected
map/render:rejected
map/data:rejected
map/debug:rejected
list/render:rejected
list/data:rejected
list/debug:rejected
property/render:rejected
property/data:rejected
property/debug:rejected
reference/render:rejected
reference/data:rejected
reference/debug:rejected
recovered
