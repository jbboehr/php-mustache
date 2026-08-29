--TEST--
Mustache::parse() owns compiled strings and template wrappers
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
class MutableApplicationTemplate extends MustacheTemplate
{
    public function replaceSource($source)
    {
        $this->template = $source;
    }
}

$mustache = new Mustache();
$mustache->setStartSequence('<%');
$mustache->setStopSequence('%>');

$source = '<%value%>|{{value}}';
$sourceFirst = $mustache->parse($source);
$sourceSecond = $mustache->parse($source);
$template = new MutableApplicationTemplate($source);
$templateFirst = $mustache->parse($template);
$templateSecond = $mustache->parse($template);

foreach (array($sourceFirst, $sourceSecond, $templateFirst, $templateSecond) as $ast) {
    var_dump(get_class($ast));
}
var_dump($sourceFirst !== $sourceSecond);
var_dump($templateFirst !== $templateSecond);
var_dump($sourceFirst->toBinary() === $sourceSecond->toBinary());
var_dump($sourceFirst->toBinary() === $templateFirst->toBinary());
var_dump($mustache->render($sourceFirst, array('value' => 'custom')));
var_dump($mustache->render($templateFirst, array('value' => 'custom')));

// Neither mutating the wrapper nor changing the compiler affects owned ASTs.
$template->replaceSource('changed:<%value%>');
$mustache->setStartSequence('{{');
$mustache->setStopSequence('}}');
var_dump($mustache->render($templateFirst, array('value' => 'later')));
var_dump($mustache->render($mustache->parse($template), array('value' => 'later')));
var_dump($mustache->render($mustache->parse($source), array('value' => 'default')));

$emptyFirst = $mustache->parse('');
$emptySecond = $mustache->parse('');
var_dump(get_class($emptyFirst));
var_dump($emptyFirst !== $emptySecond);
var_dump($mustache->render($emptyFirst, array()));
?>
--EXPECT--
string(11) "MustacheAST"
string(11) "MustacheAST"
string(11) "MustacheAST"
string(11) "MustacheAST"
bool(true)
bool(true)
bool(true)
bool(true)
string(16) "custom|{{value}}"
string(16) "custom|{{value}}"
string(15) "later|{{value}}"
string(17) "changed:<%value%>"
string(17) "<%value%>|default"
string(11) "MustacheAST"
bool(true)
string(0) ""
