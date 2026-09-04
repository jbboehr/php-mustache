--TEST--
MustacheTemplate::__toString() preserves exceptions, temporary lifetimes, and the uninitialized-value sentinel
--SKIPIF--
<?php
if (!extension_loaded('mustache') || !class_exists('WeakReference')) die('skip ');
?>
--FILE--
<?php
class UnsetTemplate extends MustacheTemplate
{
    public function clearTemplate(): void
    {
        unset($this->template);
    }
}

class ThrowingUnsetTemplate extends UnsetTemplate
{
    public function __get($name)
    {
        throw new RuntimeException('read failed');
    }
}

class TemporaryTemplateValue
{
    public function __construct(private string $mode)
    {
    }

    public function __toString(): string
    {
        echo "convert {$this->mode}\n";
        if ($this->mode === 'conversion exception') {
            throw new RuntimeException('conversion failed');
        }

        return 'temporary value';
    }

    public function __destruct()
    {
        echo "destroy {$this->mode}\n";
        if ($this->mode === 'destructor exception') {
            throw new RuntimeException('destructor failed');
        }
    }
}

class TemporaryValueTemplate extends UnsetTemplate
{
    public string $mode = 'successful conversion';
    public ?WeakReference $lastValue = null;

    public function __get($name)
    {
        $value = new TemporaryTemplateValue($this->mode);
        $this->lastValue = WeakReference::create($value);
        return $value;
    }
}

class ArrayValueTemplate extends UnsetTemplate
{
    public function __get($name)
    {
        return [];
    }
}

class ReferencedValueTemplate extends UnsetTemplate
{
    private string $fallback;

    public function setFallback(string $value): void
    {
        $this->fallback = $value;
    }

    public function &__get($name)
    {
        return $this->fallback;
    }
}

function showMissingProperty(string $label): void
{
    $value = (new stdClass())->missing;
    echo $label, ': ';
    var_dump($value);
}

set_error_handler(static function () {
    return true;
});

showMissingProperty('before');

$template = new UnsetTemplate('value');
$template->clearTemplate();
var_dump($template->__toString());
var_dump($template->__toString());

showMissingProperty('after empty reads');

$temporaryTemplate = new TemporaryValueTemplate('value');
$temporaryTemplate->clearTemplate();
var_dump($temporaryTemplate->__toString());
var_dump($temporaryTemplate->lastValue->get());

showMissingProperty('after successful temporary');

$temporaryTemplate->mode = 'conversion exception';
try {
    $temporaryTemplate->__toString();
} catch (Throwable $error) {
    echo get_class($error), ': ', $error->getMessage(), "\n";
}
var_dump($temporaryTemplate->lastValue->get());

showMissingProperty('after conversion exception');

$temporaryTemplate->mode = 'destructor exception';
try {
    $temporaryTemplate->__toString();
} catch (Throwable $error) {
    echo get_class($error), ': ', $error->getMessage(), "\n";
}
var_dump($temporaryTemplate->lastValue->get());

showMissingProperty('after destructor exception');

$arrayTemplate = new ArrayValueTemplate('value');
$arrayTemplate->clearTemplate();
set_error_handler(static function (int $severity, string $message) {
    throw new ErrorException($message, 0, $severity);
});
try {
    $arrayTemplate->__toString();
} catch (Throwable $error) {
    echo get_class($error), ': ', $error->getMessage(), "\n";
}
restore_error_handler();

showMissingProperty('after warning exception');

$referencedTemplate = new ReferencedValueTemplate('value');
$referencedTemplate->setFallback(str_repeat('owned-', 32));
$referencedTemplate->clearTemplate();
$ownedResult = $referencedTemplate->__toString();
$referencedTemplate->setFallback(str_repeat('other-', 32));
var_dump($ownedResult === str_repeat('owned-', 32));

showMissingProperty('after referenced value');

$throwingTemplate = new ThrowingUnsetTemplate('value');
$throwingTemplate->clearTemplate();
try {
    $throwingTemplate->__toString();
} catch (Throwable $error) {
    echo get_class($error), ': ', $error->getMessage(), "\n";
}

showMissingProperty('after property-read exception');

var_dump($template->__toString());
showMissingProperty('after recovery');

restore_error_handler();
?>
--EXPECT--
before: NULL
string(0) ""
string(0) ""
after empty reads: NULL
convert successful conversion
destroy successful conversion
string(15) "temporary value"
NULL
after successful temporary: NULL
convert conversion exception
destroy conversion exception
RuntimeException: conversion failed
NULL
after conversion exception: NULL
convert destructor exception
destroy destructor exception
RuntimeException: destructor failed
NULL
after destructor exception: NULL
ErrorException: Array to string conversion
after warning exception: NULL
bool(true)
after referenced value: NULL
RuntimeException: read failed
after property-read exception: NULL
string(0) ""
after recovery: NULL
