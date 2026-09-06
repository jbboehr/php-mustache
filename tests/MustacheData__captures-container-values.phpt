--TEST--
Mustache captures immediate container values before initializing their children
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
if (PHP_VERSION_ID < 80400) die('skip lazy objects require PHP 8.4');
?>
--FILE--
<?php
class CaptureTrigger
{
    public bool $ready = true;
}

class CaptureContext
{
    public object $initialize;
    public string $label;
    public object $later;
}

$mustache = new Mustache();
$source = '{{label}}/{{later.label}}';
$ast = $mustache->parse($source);
$reflection = new ReflectionClass(CaptureTrigger::class);
foreach (['ghost', 'proxy'] as $mode) {
    foreach (['map', 'object', 'list'] as $shape) {
        $paths = $shape === 'list' ? ['data', 'debug'] : ['data', 'debug', 'source', 'ast'];
        foreach ($paths as $path) {
            $label = 'before';
            $later = (object) ['label' => 'before'];
            $attempts = 0;
            $initializer = static function (CaptureTrigger $object) use (&$label, $later, &$attempts, $mode) {
                $attempts++;
                $label = 'after';
                $later->label = 'after';
                if ($mode === 'proxy') {
                    return new CaptureTrigger();
                }
                $object->ready = true;
            };
            $trigger = $mode === 'ghost'
                ? $reflection->newLazyGhost($initializer)
                : $reflection->newLazyProxy($initializer);

            if ($shape === 'object') {
                $input = new CaptureContext();
                $input->initialize = $trigger;
                $input->label =& $label;
                $input->later = $later;
            } elseif ($shape === 'map') {
                $input = ['initialize' => $trigger, 'label' => &$label, 'later' => $later];
            } else {
                $input = [$trigger, &$label, $later];
            }

            if ($path === 'source' || $path === 'ast') {
                $result = $mustache->render($path === 'source' ? $source : $ast, $input);
            } else {
                $values = $path === 'data'
                    ? (new MustacheData($input))->toValue()
                    : $mustache->debugDataStructure($input);
                $result = $shape === 'list'
                    ? $values[1] . '/' . $values[2]['label']
                    : $values['label'] . '/' . $values['later']['label'];
            }
            echo "$mode/$shape/$path: $result/$label/$attempts\n";
        }
    }
}

echo "nested container snapshot\n";
foreach (['ghost', 'proxy'] as $mode) {
    $laterArray = ['label' => 'before'];
    $laterObject = (object) ['label' => 'before'];
    $initializer = static function (CaptureTrigger $object) use (&$laterArray, $laterObject, $mode) {
        $laterArray['label'] = 'after';
        $laterObject->label = 'after';
        if ($mode === 'proxy') {
            return new CaptureTrigger();
        }
        $object->ready = true;
    };
    $trigger = $mode === 'ghost'
        ? $reflection->newLazyGhost($initializer)
        : $reflection->newLazyProxy($initializer);

    $values = (new MustacheData([$trigger, &$laterArray, $laterObject]))->toValue();
    echo "$mode: ", $values[1]['label'], '/', $values[2]['label'], '/',
        $laterArray['label'], '/', $laterObject->label, "\n";
}
?>
--EXPECT--
ghost/map/data: before/after/after/1
ghost/map/debug: before/after/after/1
ghost/map/source: before/after/after/1
ghost/map/ast: before/after/after/1
ghost/object/data: before/after/after/1
ghost/object/debug: before/after/after/1
ghost/object/source: before/after/after/1
ghost/object/ast: before/after/after/1
ghost/list/data: before/after/after/1
ghost/list/debug: before/after/after/1
proxy/map/data: before/after/after/1
proxy/map/debug: before/after/after/1
proxy/map/source: before/after/after/1
proxy/map/ast: before/after/after/1
proxy/object/data: before/after/after/1
proxy/object/debug: before/after/after/1
proxy/object/source: before/after/after/1
proxy/object/ast: before/after/after/1
proxy/list/data: before/after/after/1
proxy/list/debug: before/after/after/1
nested container snapshot
ghost: before/after/after/after
proxy: before/after/after/after
