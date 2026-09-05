--TEST--
Mustache distinguishes uninitialized template wrappers from initialized empty source
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
function expectValueError($label, $callback)
{
    try {
        $callback();
        echo $label, ": accepted\n";
    } catch (Throwable $error) {
        echo $label, ': ', get_class($error), "\n";
    }
}

$mustache = new Mustache();
$partialTemplate = $mustache->parse('A{{>empty}}B');
$templates = [
    'omitted' => new MustacheTemplate(),
    'null' => new MustacheTemplate(null),
];

foreach ($templates as $label => $template) {
    expectValueError($label . ' render', fn() => $mustache->render($template, []));
    expectValueError($label . ' parse', fn() => $mustache->parse($template));
    expectValueError($label . ' source partial', fn() => $mustache->render('A{{>empty}}B', [], ['empty' => $template]));
    expectValueError($label . ' AST partial', fn() => $mustache->render($partialTemplate, [], ['empty' => $template]));
}

foreach (['source' => 'A{{>empty}}B', 'AST' => $partialTemplate] as $label => $template) {
    expectValueError(
        $label . ' invalid after empty partial',
        fn() => $mustache->render($template, [], [
            'empty' => new MustacheTemplate(''),
            'invalid' => 123,
        ])
    );
}

echo $mustache->render('still {{state}}', ['state' => 'usable']), "\n";
?>
--EXPECT--
omitted render: ValueError
omitted parse: ValueError
omitted source partial: ValueError
omitted AST partial: ValueError
null render: ValueError
null parse: ValueError
null source partial: ValueError
null AST partial: ValueError
source invalid after empty partial: ValueError
AST invalid after empty partial: ValueError
still usable
