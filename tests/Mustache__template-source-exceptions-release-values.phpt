--TEST--
Mustache releases rejected template getter values and preserves PHP exceptions
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
class RejectedTemplateSource
{
    public static $destructions = 0;
    public static $throwOnDestruct = false;

    public function __destruct()
    {
        self::$destructions++;
        if (self::$throwOnDestruct) {
            throw new RuntimeException('cleanup failed');
        }
    }
}

class FailingGetterTemplate extends MustacheTemplate
{
    public $mode;
    public $last;
    public $failure;

    public function __construct($mode)
    {
        $this->mode = $mode;
        $this->failure = new RuntimeException('getter failed');
        unset($this->template);
    }

    public function __get($name)
    {
        if ($this->mode === 'getter') {
            throw $this->failure;
        }
        $value = new RejectedTemplateSource();
        $this->last = WeakReference::create($value);
        return $value;
    }
}

$mustache = new Mustache();
$partialRoot = $mustache->parse('{{>p}}');
$operations = array(
    'parse' => fn($template) => $mustache->parse($template),
    'render' => fn($template) => $mustache->render($template, array()),
    'source partial' => fn($template) => $mustache->render('{{>p}}', array(), array('p' => $template)),
    'AST partial' => fn($template) => $mustache->render($partialRoot, array(), array('p' => $template)),
);

foreach ($operations as $name => $operation) {
    foreach (array('getter', 'rejected', 'cleanup') as $mode) {
        $template = new FailingGetterTemplate($mode);
        $before = RejectedTemplateSource::$destructions;
        RejectedTemplateSource::$throwOnDestruct = $mode === 'cleanup';
        echo $name, '/', $mode, ': ';
        try {
            $operation($template);
            echo "no exception\n";
        } catch (Throwable $e) {
            if ($mode === 'getter') {
                echo $e === $template->failure ? 'original exception' : get_class($e);
            } else {
                echo get_class($e);
                if ($mode === 'cleanup') {
                    echo ': ', $e->getMessage(), ', previous=',
                        $e->getPrevious() === null ? 'none' : get_class($e->getPrevious());
                }
            }
            echo "\n";
        } finally {
            RejectedTemplateSource::$throwOnDestruct = false;
        }
        if ($mode !== 'getter') {
            echo 'released=', $template->last->get() === null ? 'yes' : 'no',
                ', destroyed=', RejectedTemplateSource::$destructions - $before, "\n";
        }
    }
    echo 'recovery: ', $mustache->render('Hello {{name}}', array('name' => 'Ada')), "\n";
}
?>
--EXPECT--
parse/getter: original exception
parse/rejected: ValueError
released=yes, destroyed=1
parse/cleanup: RuntimeException: cleanup failed, previous=ValueError
released=yes, destroyed=1
recovery: Hello Ada
render/getter: original exception
render/rejected: ValueError
released=yes, destroyed=1
render/cleanup: RuntimeException: cleanup failed, previous=ValueError
released=yes, destroyed=1
recovery: Hello Ada
source partial/getter: original exception
source partial/rejected: ValueError
released=yes, destroyed=1
source partial/cleanup: RuntimeException: cleanup failed, previous=ValueError
released=yes, destroyed=1
recovery: Hello Ada
AST partial/getter: original exception
AST partial/rejected: ValueError
released=yes, destroyed=1
AST partial/cleanup: RuntimeException: cleanup failed, previous=ValueError
released=yes, destroyed=1
recovery: Hello Ada
