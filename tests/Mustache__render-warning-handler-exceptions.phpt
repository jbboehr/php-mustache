--TEST--
Mustache preserves warning-handler exceptions and skips later PHP callbacks
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$warningData = new MustacheData([static fn() => 'lambda still usable']);

foreach (['source', 'AST'] as $mode) {
    echo $mode, "\n";
    $mustache = new Mustache();
    $source = '{{#first}}{{name}}{{/first}}|{{#later}}unused{{/later}}';
    $template = $mode === 'AST' ? $mustache->parse($source) : $source;
    $entered = 0;
    $afterWarning = 0;
    $later = 0;
    $warnings = [];
    $expected = new ErrorException('warning converted to exception', 0, E_WARNING);
    $data = [
        'name' => 'Ada',
        'first' => function ($text, MustacheLambdaHelper $helper) use ($warningData, &$entered, &$afterWarning) {
            ++$entered;
            $warningData->toValue();
            ++$afterWarning;
            return $helper->render($text);
        },
        'later' => function () use (&$later) {
            ++$later;
            return 'ok';
        },
    ];

    set_error_handler(static function ($severity, $message) use (&$warnings, $expected) {
        $warnings[] = [$severity, $message];
        throw $expected;
    });
    $result = 'unchanged';
    try {
        $result = $mustache->render($template, $data);
        echo "render did not throw\n";
    } catch (Throwable $caught) {
        echo 'same exception: ';
        var_dump($caught === $expected);
    } finally {
        restore_error_handler();
    }

    echo 'expected warning: ';
    var_dump($warnings === [[E_WARNING, 'Lambda data cannot be converted to a PHP value']]);
    echo 'result unchanged: ';
    var_dump($result === 'unchanged');
    echo 'callbacks: entered=', $entered, ' after=', $afterWarning, ' later=', $later, "\n";

    $data['name'] = 'Grace';
    $data['first'] = static fn($text, MustacheLambdaHelper $helper) => $helper->render($text);
    echo 'recovery: ', $mustache->render($template, $data), "\n";
    echo 'later after recovery: ', $later, "\n";
    echo 'warning data: ', $mustache->render('{{#.}}{{.}}{{/.}}', $warningData), "\n";
}
?>
--EXPECT--
source
same exception: bool(true)
expected warning: bool(true)
result unchanged: bool(true)
callbacks: entered=1 after=0 later=0
recovery: Grace|ok
later after recovery: 1
warning data: lambda still usable
AST
same exception: bool(true)
expected warning: bool(true)
result unchanged: bool(true)
callbacks: entered=1 after=0 later=0
recovery: Grace|ok
later after recovery: 1
warning data: lambda still usable
