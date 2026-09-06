--TEST--
Mustache preserves ordinary and invokable method lambdas, destructor-named properties, and normal destruction
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
class MethodLifecycleContext
{
    public static int $destructions = 0;
    public string $name = 'Ada';
    public string $__destruct = 'property';

    public function Greeting()
    {
        return 'hello';
    }

    public function __toString()
    {
        return 'text';
    }

    public function __destruct()
    {
        ++self::$destructions;
    }
}

class InvokableLifecycleContext
{
    public static int $destructions = 0;

    public function __invoke()
    {
        return 'invoked';
    }

    public function __destruct()
    {
        ++self::$destructions;
    }
}

$mustache = new Mustache();
$source = '{{name}}|{{Greeting}}|{{__destruct}}|{{__toString}}';
foreach (['source', 'AST'] as $backend) {
    $template = $backend === 'AST' ? $mustache->parse($source) : $source;
    foreach (['object', 'MustacheData'] as $input) {
        $before = MethodLifecycleContext::$destructions;
        $object = new MethodLifecycleContext();
        $weak = WeakReference::create($object);
        $data = $input === 'MustacheData' ? new MustacheData($object) : $object;
        unset($object);

        echo "$backend/$input: ", $mustache->render($template, $data), "\n";
        echo 'retained=', $weak->get() !== null ? 'yes' : 'no',
            '; destructors=', MethodLifecycleContext::$destructions - $before, "\n";
        unset($data);
        echo 'released=', $weak->get() === null ? 'yes' : 'no',
            '; destructors=', MethodLifecycleContext::$destructions - $before, "\n";
    }
}

$object = new InvokableLifecycleContext();
$weak = WeakReference::create($object);
$data = new MustacheData(['callback' => $object]);
unset($object);

echo 'invokable: ', $mustache->render('{{callback}}', $data), "\n";
echo 'retained=', $weak->get() !== null ? 'yes' : 'no',
    '; destructors=', InvokableLifecycleContext::$destructions, "\n";
unset($data);
echo 'released=', $weak->get() === null ? 'yes' : 'no',
    '; destructors=', InvokableLifecycleContext::$destructions, "\n";
?>
--EXPECT--
source/object: Ada|hello|property|text
retained=yes; destructors=0
released=yes; destructors=1
source/MustacheData: Ada|hello|property|text
retained=yes; destructors=0
released=yes; destructors=1
AST/object: Ada|hello|property|text
retained=yes; destructors=0
released=yes; destructors=1
AST/MustacheData: Ada|hello|property|text
retained=yes; destructors=0
released=yes; destructors=1
invokable: invoked
retained=yes; destructors=0
released=yes; destructors=1
