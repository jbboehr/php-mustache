--TEST--
MustacheData keeps copied properties while retained methods read current object state
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
class ChangingMethodContext
{
    public string $name = 'Ada';

    public function currentName(): string
    {
        return $this->name;
    }
}

$context = new ChangingMethodContext();
$data = new MustacheData($context);
$mustache = new Mustache();
$template = '{{name}}|{{currentName}}';

foreach (['Lin', 'Grace'] as $name) {
    $context->name = $name;
    echo $mustache->render($template, $data), "\n";
}
echo $mustache->render($template, $context), "\n";
?>
--EXPECT--
Ada|Lin
Ada|Grace
Grace|Grace
