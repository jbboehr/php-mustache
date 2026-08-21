--TEST--
Mustache reports parser errors with source coordinates and remains reusable
--INI--
error_reporting=E_ALL
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
function reportParserError($label, $callback, $messageNeedle = 'Mismatched closing section') {
  try {
    $callback();
    echo $label, ":no exception\n";
  } catch (MustacheParserException $e) {
    printf(
      "%s:%s:%s:%d:%d\n",
      $label,
      get_class($e),
      get_parent_class($e),
      $e->templateLineNo,
      $e->templateCharNo
    );
    var_dump(strpos($e->getMessage(), $messageNeedle) !== false);
  }
}

$mustache = new Mustache();
$invalid = "first line\n{{#a}}{{/b}}";
$manual = new MustacheParserException('manual');
printf("default:%d:%d\n", $manual->templateLineNo, $manual->templateCharNo);

reportParserError('parse', function () use ($mustache, $invalid) {
  $mustache->parse($invalid);
});
reportParserError('tokenize', function () use ($mustache, $invalid) {
  $mustache->tokenize($invalid);
});
reportParserError('render-source', function () use ($mustache, $invalid) {
  $mustache->render($invalid, array());
});
reportParserError('literal-percent', function () use ($mustache) {
  $mustache->render('{{#a}}{{/%s}}', array());
}, "'%s'");
reportParserError('unknown-location', function () use ($mustache) {
  $mustache->render('{{#a}}', array());
}, 'Unclosed section');
reportParserError('render-source-partial', function () use ($mustache, $invalid) {
  $mustache->render('{{>broken}}', array(), array('broken' => $invalid));
});

$ast = $mustache->parse('{{>broken}}');
reportParserError('render-ast-partial', function () use ($mustache, $ast, $invalid) {
  $mustache->render($ast, array(), array('broken' => $invalid));
});
reportParserError('lambda-helper', function () use ($mustache, $invalid) {
  $mustache->render('{{#lambda}}ignored{{/lambda}}', array(
    'lambda' => function ($text, MustacheLambdaHelper $helper) use ($invalid) {
      return $helper->render($invalid);
    },
  ));
});
reportParserError('lambda-helper-aborts', function () use ($mustache, $invalid) {
  $mustache->render(
    '{{#lambda}}ignored{{/lambda}}{{>loop}}',
    array(
      'lambda' => function ($text, MustacheLambdaHelper $helper) use ($invalid) {
        return $helper->render($invalid);
      },
    ),
    array('loop' => '{{>loop}}')
  );
});

var_dump($mustache->render('{{value}}', array('value' => 'reused')));
?>
--EXPECT--
default:-1:-1
parse:MustacheParserException:MustacheException:2:7
bool(true)
tokenize:MustacheParserException:MustacheException:2:7
bool(true)
render-source:MustacheParserException:MustacheException:2:7
bool(true)
literal-percent:MustacheParserException:MustacheException:1:7
bool(true)
unknown-location:MustacheParserException:MustacheException:-1:-1
bool(true)
render-source-partial:MustacheParserException:MustacheException:2:7
bool(true)
render-ast-partial:MustacheParserException:MustacheException:2:7
bool(true)
lambda-helper:MustacheParserException:MustacheException:2:7
bool(true)
lambda-helper-aborts:MustacheParserException:MustacheException:2:7
bool(true)
string(6) "reused"
