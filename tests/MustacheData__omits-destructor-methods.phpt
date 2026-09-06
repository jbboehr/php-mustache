--TEST--
MustacheData omits destructor methods and releases property-only source objects
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
class DestructorCalls
{
    public static int $count = 0;
}

class DestructorContext
{
    public string $name = 'Ada';

    public function __construct() {}

    public function __destruct()
    {
        ++DestructorCalls::$count;
    }

    public static function staticValue()
    {
        return 'static';
    }
}

class InheritedDestructorContext extends DestructorContext {}

class MixedCaseDestructorContext
{
    public string $name = 'Ada';

    public function __DeStRuCt()
    {
        ++DestructorCalls::$count;
    }
}

trait DestructorTrait
{
    public function __destruct()
    {
        ++DestructorCalls::$count;
    }
}

class TraitDestructorContext
{
    use DestructorTrait;

    public string $name = 'Ada';
}

$mustache = new Mustache();
foreach ([
    DestructorContext::class,
    InheritedDestructorContext::class,
    MixedCaseDestructorContext::class,
    TraitDestructorContext::class,
] as $class) {
    $before = DestructorCalls::$count;
    $object = new $class();
    $weak = WeakReference::create($object);
    $data = new MustacheData($object);
    $warnings = 0;
    set_error_handler(static function () use (&$warnings) {
        ++$warnings;
        return true;
    });
    try {
        $value = $data->toValue();
    } finally {
        restore_error_handler();
    }
    echo $class, ': ', implode(',', array_keys($value)), "; warnings=$warnings\n";

    unset($object);
    echo 'released=', $weak->get() === null ? 'yes' : 'no',
        '; destructors=', DestructorCalls::$count - $before, "\n";
    echo 'render=', $mustache->render('{{name}}', $data), "\n";
    unset($data);
    echo 'after data release=', DestructorCalls::$count - $before, "\n";
}
?>
--EXPECT--
DestructorContext: name; warnings=0
released=yes; destructors=1
render=Ada
after data release=1
InheritedDestructorContext: name; warnings=0
released=yes; destructors=1
render=Ada
after data release=1
MixedCaseDestructorContext: name; warnings=0
released=yes; destructors=1
render=Ada
after data release=1
TraitDestructorContext: name; warnings=0
released=yes; destructors=1
render=Ada
after data release=1
