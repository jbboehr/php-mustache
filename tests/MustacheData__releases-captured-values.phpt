--TEST--
Mustache releases captured objects and aliases after successful and failed conversion
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
class CapturedRecord
{
    public string $name = 'Ada';
}

$mustache = new Mustache();
$resource = fopen('php://memory', 'r');
foreach (['array', 'object'] as $shape) {
    foreach (['success', 'failure'] as $outcome) {
        $record = new CapturedRecord();
        $shared = $record;
        $weak = WeakReference::create($record);
        $input = ['a' => $record, 'b' => &$shared, 'c' => &$shared, 'd' => $record, 'e' => $record];
        if ($outcome === 'failure') {
            $input['invalid'] = $resource;
        }
        if ($shape === 'object') {
            $input = (object) $input;
        }

        echo "$shape/$outcome\n";
        if ($outcome === 'success') {
            $data = new MustacheData($input);
            var_dump($mustache->render('{{a.name}}/{{b.name}}/{{c.name}}/{{d.name}}/{{e.name}}', $data));
        } else {
            try {
                $mustache->render('unused', $input);
                echo "exception missing\n";
            } catch (ValueError $error) {
                echo "ValueError\n";
            }
            unset($error); // Exception traces can retain the input argument.
        }
        var_dump($weak->get() === $record);
        unset($record, $shared, $input);
        gc_collect_cycles();
        var_dump($weak->get());
        if ($outcome === 'success') {
            var_dump($mustache->render('{{a.name}}', $data));
            unset($data);
        }
    }
}
fclose($resource);
?>
--EXPECT--
array/success
string(19) "Ada/Ada/Ada/Ada/Ada"
bool(true)
NULL
string(3) "Ada"
array/failure
ValueError
bool(true)
NULL
object/success
string(19) "Ada/Ada/Ada/Ada/Ada"
bool(true)
NULL
string(3) "Ada"
object/failure
ValueError
bool(true)
NULL
