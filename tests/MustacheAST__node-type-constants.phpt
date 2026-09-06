--TEST--
MustacheAST node constants identify parsed nodes and comment contents
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
?>
--FILE--
<?php
$names = [
    'NODE_NONE', 'NODE_ROOT', 'NODE_OUTPUT', 'NODE_TAG', 'NODE_CONTAINER',
    'NODE_VARIABLE', 'NODE_NEGATE', 'NODE_SECTION', 'NODE_STOP',
    'NODE_COMMENT', 'NODE_PARTIAL', 'NODE_INLINE_PARTIAL',
];
$class = new ReflectionClass(MustacheAST::class);
$types = [];
foreach ($names as $name) {
    $constant = $class->getReflectionConstant($name);
    if ($constant === false) {
        echo "Missing $name\n";
        exit;
    }
    $types[$name] = $constant->getValue();
    printf("%s=%s %s %s\n", $name, $types[$name],
        $constant->isPublic() ? 'public' : 'nonpublic', gettype($types[$name]));
}

function describeNodes(array $node, array $types): array
{
    $names = [array_search($node['type'], $types, true) ?: 'unknown'];
    foreach ($node['children'] ?? [] as $child) {
        $names = array_merge($names, describeNodes($child, $types));
    }
    return $names;
}

function comments(array $node): array
{
    $result = $node['type'] === MustacheAST::NODE_COMMENT ? [$node['data'] ?? ''] : [];
    foreach ($node['children'] ?? [] as $child) {
        $result = array_merge($result, comments($child));
    }
    return $result;
}

$mustache = new Mustache();
$source = 'text{{name}}{{{raw}}}{{^empty}}fallback{{/empty}}'
    . '{{#show}}shown{{/show}}{{! asset: main.css }}{{>piece}}';
foreach ([
    'tokenize' => $mustache->tokenize($source),
    'toArray' => $mustache->parse($source)->toArray(),
] as $method => $tree) {
    echo $method, ': ', implode(' ', describeNodes($tree, $types)), "\n";
}

$source = "{{!\nassets:\n  - main.css\n  - main.js\n}}\n"
    . '{{#show}}{{! nested }}{{/show}}{{=<% %>=}}<%! custom %><%! %>';
foreach ([
    'tokenize' => $mustache->tokenize($source),
    'toArray' => $mustache->parse($source)->toArray(),
] as $method => $tree) {
    echo $method, ' comments: ', json_encode(comments($tree)), "\n";
}
?>
--EXPECT--
NODE_NONE=0 public integer
NODE_ROOT=1 public integer
NODE_OUTPUT=2 public integer
NODE_TAG=4 public integer
NODE_CONTAINER=8 public integer
NODE_VARIABLE=16 public integer
NODE_NEGATE=32 public integer
NODE_SECTION=64 public integer
NODE_STOP=128 public integer
NODE_COMMENT=256 public integer
NODE_PARTIAL=512 public integer
NODE_INLINE_PARTIAL=1024 public integer
tokenize: NODE_ROOT NODE_OUTPUT NODE_VARIABLE NODE_VARIABLE NODE_NEGATE NODE_OUTPUT NODE_STOP NODE_SECTION NODE_OUTPUT NODE_STOP NODE_COMMENT NODE_PARTIAL
toArray: NODE_ROOT NODE_OUTPUT NODE_VARIABLE NODE_VARIABLE NODE_NEGATE NODE_OUTPUT NODE_STOP NODE_SECTION NODE_OUTPUT NODE_STOP NODE_COMMENT NODE_PARTIAL
tokenize comments: ["assets:\n  - main.css\n  - main.js","nested","custom",""]
toArray comments: ["assets:\n  - main.css\n  - main.js","nested","custom",""]
