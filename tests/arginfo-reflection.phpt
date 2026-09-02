--TEST--
Native arginfo matches the public API contract
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
?>
--FILE--
<?php
function reflectionTypeName(?ReflectionType $type): string
{
    if ($type === null) {
        return '-';
    }

    if ($type instanceof ReflectionNamedType) {
        $names = [$type->getName()];
        if ($type->allowsNull() && $type->getName() !== 'mixed') {
            $names[] = 'null';
        }
    } else {
        $names = [];
        foreach ($type->getTypes() as $member) {
            $names[] = $member->getName();
        }
    }

    sort($names, SORT_STRING);
    return implode('|', array_unique($names));
}

function describeMethod(string $class, string $methodName): void
{
    $method = new ReflectionMethod($class, $methodName);
    $visibility = $method->isPrivate()
        ? 'private'
        : ($method->isProtected() ? 'protected' : 'public');
    $parameters = [];
    foreach ($method->getParameters() as $parameter) {
        $description = $parameter->getName() . ':' . reflectionTypeName($parameter->getType());
        if ($parameter->isDefaultValueAvailable()) {
            $description .= '=' . var_export($parameter->getDefaultValue(), true);
        } elseif ($parameter->isOptional()) {
            $description .= '=?';
        }
        $parameters[] = $description;
    }

    printf(
        "%s%s %s::%s(%s):%s\n",
        $visibility,
        $method->isStatic() ? ' static' : '',
        $class,
        $methodName,
        implode(', ', $parameters),
        reflectionTypeName($method->getReturnType()),
    );
}

$methods = [
    'Mustache' => [
        '__construct',
        'getEscapeByDefault',
        'getStartSequence',
        'getStopSequence',
        'setEscapeByDefault',
        'setStartSequence',
        'setStopSequence',
        'parse',
        'render',
        'tokenize',
        'debugDataStructure',
    ],
    'MustacheAST' => [
        '__construct',
        'fromBinary',
        '__serialize',
        '__unserialize',
        'toArray',
        'toBinary',
        '__toString',
    ],
    'MustacheTemplate' => ['__construct', '__toString'],
    'MustacheData' => ['__construct', 'toValue'],
    'MustacheLambdaHelper' => ['__construct', 'render'],
];

foreach ($methods as $class => $classMethods) {
    foreach ($classMethods as $method) {
        describeMethod($class, $method);
    }
}
?>
--EXPECT--
public Mustache::__construct():-
public Mustache::getEscapeByDefault():bool
public Mustache::getStartSequence():string
public Mustache::getStopSequence():string
public Mustache::setEscapeByDefault(escapeByDefault:bool|int):bool
public Mustache::setStartSequence(startSequence:string):bool
public Mustache::setStopSequence(stopSequence:string):bool
public Mustache::parse(tmpl:MustacheAST|MustacheTemplate|string):MustacheAST
public Mustache::render(str:MustacheAST|MustacheTemplate|string, vars:mixed, partials:array|null=NULL):string
public Mustache::tokenize(tmpl:string):array
public Mustache::debugDataStructure(vars:mixed):mixed
public MustacheAST::__construct(vars:null|string=NULL):-
public static MustacheAST::fromBinary(binary:string):static
public MustacheAST::__serialize():array
public MustacheAST::__unserialize(data:array):void
public MustacheAST::toArray():array
public MustacheAST::toBinary():string
public MustacheAST::__toString():string
public MustacheTemplate::__construct(vars:null|string=NULL):-
public MustacheTemplate::__toString():string
public MustacheData::__construct(tmpl:mixed):-
public MustacheData::toValue():mixed
private MustacheLambdaHelper::__construct():-
public MustacheLambdaHelper::render(tmpl:string):string
