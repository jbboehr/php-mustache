--TEST--
Mustache::parse() validates each input kind and remains reusable
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
function reportParseFailure($label, $callback)
{
    try {
        $callback();
        echo $label, ":accepted\n";
    } catch (Throwable $error) {
        echo $label, ':', get_class($error), "\n";
    }
}

class UninitializedApplicationTemplate extends MustacheTemplate {}
class UninitializedApplicationAST extends MustacheAST {}
class ApplicationStringable
{
    public function __toString()
    {
        return 'stringable';
    }
}

$mustache = new Mustache();
$mustache->setStartSequence('<%');
$mustache->setStopSequence('%>');

reportParseFailure('invalid-string', function () use ($mustache) {
    $mustache->parse('<%#a%><%/b%>');
});
reportParseFailure('invalid-template', function () use ($mustache) {
    $mustache->parse(new MustacheTemplate('<%#a%><%/b%>'));
});

$templateReflection = new ReflectionClass(UninitializedApplicationTemplate::class);
$astReflection = new ReflectionClass(UninitializedApplicationAST::class);
$uninitializedTemplate = $templateReflection->newInstanceWithoutConstructor();
$uninitializedAST = $astReflection->newInstanceWithoutConstructor();
reportParseFailure('uninitialized-template', function () use ($mustache, $uninitializedTemplate) {
    $mustache->parse($uninitializedTemplate);
});
reportParseFailure('uninitialized-ast', function () use ($mustache, $uninitializedAST) {
    $mustache->parse($uninitializedAST);
});

$resource = fopen('php://memory', 'r');
$unsupported = array(
    'null' => null,
    'false' => false,
    'integer' => 1,
    'float' => 1.5,
    'array' => array('template'),
    'resource' => $resource,
    'object' => new stdClass(),
    'stringable' => new ApplicationStringable(),
);
foreach ($unsupported as $label => $value) {
    reportParseFailure($label, function () use ($mustache, $value) {
        $mustache->parse($value);
    });
}
fclose($resource);

reportParseFailure('missing', function () use ($mustache) {
    $mustache->parse();
});
reportParseFailure('extra', function () use ($mustache) {
    $mustache->parse('valid', 'extra');
});

$recovered = $mustache->parse(tmpl: '<%value%>');
var_dump(get_class($recovered));
var_dump($mustache->render($recovered, array('value' => 'recovered')));
var_dump($mustache->parse(tmpl: $recovered) === $recovered);
?>
--EXPECT--
invalid-string:MustacheParserException
invalid-template:MustacheParserException
uninitialized-template:ValueError
uninitialized-ast:ValueError
null:TypeError
false:TypeError
integer:TypeError
float:TypeError
array:TypeError
resource:TypeError
object:TypeError
stringable:TypeError
missing:ArgumentCountError
extra:ArgumentCountError
string(11) "MustacheAST"
string(9) "recovered"
bool(true)
