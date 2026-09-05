--TEST--
Mustache releases temporary template sources without consuming stored sources
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
class GetterTemplate extends MustacheTemplate
{
    public $source;

    public function __construct()
    {
        // A runtime string lets us observe references retained by property reads.
        $this->source = str_repeat('Hello {{name}} ', 2);
        unset($this->template);
    }

    public function __get($name)
    {
        return $this->source;
    }
}

class StoredTemplate extends MustacheTemplate
{
    public function __construct($source)
    {
        // Assign through PHP so this property and the caller share one string.
        $this->template = $source;
    }

    public function source()
    {
        return $this->template;
    }
}

function sourceRefcount($source)
{
    ob_start();
    debug_zval_dump($source);
    $dump = ob_get_clean();
    if (!preg_match('/refcount\((\d+)\)/', $dump, $matches)) {
        throw new RuntimeException('Expected a refcounted source string');
    }
    return (int) $matches[1];
}

$mustache = new Mustache();
$partialRoot = $mustache->parse('{{>p}}');
$data = array('name' => 'Ada');
$operations = array(
    'cast' => fn($template) => (string) $template,
    'parse' => fn($template) => $mustache->render($mustache->parse($template), $data),
    'render' => fn($template) => $mustache->render($template, $data),
    'source partial' => fn($template) => $mustache->render('{{>p}}', $data, array('p' => $template)),
    'AST partial' => fn($template) => $mustache->render($partialRoot, $data, array('p' => $template)),
);

foreach ($operations as $name => $operation) {
    $template = new GetterTemplate();
    $before = sourceRefcount($template->source);
    $result = $operation($template);
    echo $name, ': ', json_encode($result), "\n";
    // The cast intentionally shares the string until its result is released.
    unset($result);
    echo 'retained=', sourceRefcount($template->source) - $before, "\n";
}

unset($operations['cast']);
foreach ($operations as $name => $operation) {
    $source = str_repeat('Stored {{name}} ', 2);
    $template = new StoredTemplate($source);
    $before = sourceRefcount($source);
    $result = $operation($template);
    echo 'stored ', $name, ': ', json_encode($result), "\n";
    unset($result);
    echo 'retained=', sourceRefcount($source) - $before,
        ', source=', $template->source() === $source ? 'preserved' : 'changed', "\n";
}
?>
--EXPECT--
cast: "Hello {{name}} Hello {{name}} "
retained=0
parse: "Hello Ada Hello Ada "
retained=0
render: "Hello Ada Hello Ada "
retained=0
source partial: "Hello Ada Hello Ada "
retained=0
AST partial: "Hello Ada Hello Ada "
retained=0
stored parse: "Stored Ada Stored Ada "
retained=0, source=preserved
stored render: "Stored Ada Stored Ada "
retained=0, source=preserved
stored source partial: "Stored Ada Stored Ada "
retained=0, source=preserved
stored AST partial: "Stored Ada Stored Ada "
retained=0, source=preserved
