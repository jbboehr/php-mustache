--TEST--
Mustache and MustacheData reject unsupported serialization
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
class ApplicationMustache extends Mustache {}
class ApplicationMustacheData extends MustacheData {}

if (PHP_VERSION_ID < 80100) {
    class SerializingMustache extends Mustache {}
    class SerializingMustacheData extends MustacheData {}
} else {
    class SerializingMustache extends Mustache
    {
        public function __serialize(): array
        {
            return array();
        }

        public function __unserialize(array $data): void
        {
        }
    }

    class SerializingMustacheData extends MustacheData
    {
        public function __serialize(): array
        {
            return array();
        }

        public function __unserialize(array $data): void
        {
        }
    }
}

function reportRejection($label, $callback)
{
    set_error_handler(function () {
        return true;
    });
    try {
        $result = $callback();
        echo $label, $result === false ? ":rejected\n" : ":accepted\n";
    } catch (Throwable $error) {
        echo $label, ":rejected\n";
    } finally {
        restore_error_handler();
    }
}

function reportProtectedHooks($class)
{
    if (PHP_VERSION_ID >= 80100) {
        echo 'hooks-', $class, ":protected\n";
        return;
    }

    try {
        $serialize = new ReflectionMethod($class, '__serialize');
        $unserialize = new ReflectionMethod($class, '__unserialize');
        $protected = $serialize->isFinal() && $unserialize->isFinal();
    } catch (ReflectionException $error) {
        $protected = false;
    }
    echo 'hooks-', $class, $protected ? ":protected\n" : ":overrideable\n";
}

reportProtectedHooks(Mustache::class);
reportProtectedHooks(MustacheData::class);

$values = array(
    'Mustache' => new Mustache(),
    'ApplicationMustache' => new ApplicationMustache(),
    'SerializingMustache' => new SerializingMustache(),
    'MustacheData' => new MustacheData(array('name' => 'Ada')),
    'ApplicationMustacheData' => new ApplicationMustacheData(array('name' => 'Ada')),
    'SerializingMustacheData' => new SerializingMustacheData(array('name' => 'Ada')),
);

foreach ($values as $class => $value) {
    reportRejection('serialize-' . $class, function () use ($value) {
        return serialize($value);
    });
}

foreach (array_keys($values) as $class) {
    reportRejection('unserialize-' . $class, function () use ($class) {
        return unserialize(sprintf('O:%d:"%s":0:{}', strlen($class), $class));
    });
}
?>
--EXPECT--
hooks-Mustache:protected
hooks-MustacheData:protected
serialize-Mustache:rejected
serialize-ApplicationMustache:rejected
serialize-SerializingMustache:rejected
serialize-MustacheData:rejected
serialize-ApplicationMustacheData:rejected
serialize-SerializingMustacheData:rejected
unserialize-Mustache:rejected
unserialize-ApplicationMustache:rejected
unserialize-SerializingMustache:rejected
unserialize-MustacheData:rejected
unserialize-ApplicationMustacheData:rejected
unserialize-SerializingMustacheData:rejected
