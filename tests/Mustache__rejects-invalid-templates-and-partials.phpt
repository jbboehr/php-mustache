--TEST--
Mustache rejects invalid templates and partials on source and AST render paths
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
function printException($label, $callback) {
  $warnings = array();
  set_error_handler(function ($severity, $message) use (&$warnings) {
    $warnings[] = $message;
    return true;
  });

  try {
    $callback();
    echo $label, ":no exception warnings=", count($warnings), "\n";
  } catch (Throwable $error) {
    echo $label, ':', get_class($error), ' warnings=', count($warnings), "\n";
  } finally {
    restore_error_handler();
  }
}

$mustache = new Mustache();

printException('render-int', function () use ($mustache) {
  $mustache->render(123, []);
});
printException('render-object', function () use ($mustache) {
  $mustache->render(new stdClass(), []);
});
printException('render-empty-template', function () use ($mustache) {
  $mustache->render(new MustacheTemplate(), []);
});
printException('parse-int', function () use ($mustache) {
  $mustache->parse(123);
});
printException('parse-object', function () use ($mustache) {
  $mustache->parse(new stdClass());
});

$emptyAST = (new ReflectionClass(MustacheAST::class))->newInstanceWithoutConstructor();
printException('render-empty-ast', function () use ($mustache, $emptyAST) {
  $mustache->render($emptyAST, []);
});
printException('parse-empty-ast', function () use ($mustache, $emptyAST) {
  $mustache->parse($emptyAST);
});

$source = 'A{{>x}}B';
$ast = $mustache->parse($source);
foreach (['source' => $source, 'ast' => $ast] as $kind => $template) {
  printException($kind . '-numeric-partial', function () use ($mustache, $template) {
    $mustache->render($template, [], ['numeric partial']);
  });
  printException($kind . '-integer-partial', function () use ($mustache, $template) {
    $mustache->render($template, [], ['x' => 123]);
  });
  printException($kind . '-object-partial', function () use ($mustache, $template) {
    $mustache->render($template, [], ['x' => new stdClass()]);
  });
  printException($kind . '-empty-template-partial', function () use ($mustache, $template) {
    $mustache->render($template, [], ['x' => new MustacheTemplate()]);
  });
}

// Validation must not stop after the first valid AST partial.
printException('valid-ast-before-invalid', function () use ($mustache, $source, $ast) {
  $mustache->render($source, [], ['unused' => $ast, 'x' => 123]);
});

// An AST partial selects the compatibility renderer even for a source template.
printException('empty-ast-partial', function () use ($mustache, $source, $emptyAST) {
  $mustache->render($source, [], ['x' => $emptyAST]);
});

var_dump($mustache->render($source, [], ['x' => 'valid']));
?>
--EXPECT--
render-int:TypeError warnings=0
render-object:TypeError warnings=0
render-empty-template:ValueError warnings=0
parse-int:TypeError warnings=0
parse-object:TypeError warnings=0
render-empty-ast:ValueError warnings=0
parse-empty-ast:ValueError warnings=0
source-numeric-partial:ValueError warnings=0
source-integer-partial:ValueError warnings=0
source-object-partial:ValueError warnings=0
source-empty-template-partial:ValueError warnings=0
ast-numeric-partial:ValueError warnings=0
ast-integer-partial:ValueError warnings=0
ast-object-partial:ValueError warnings=0
ast-empty-template-partial:ValueError warnings=0
valid-ast-before-invalid:ValueError warnings=0
empty-ast-partial:ValueError warnings=0
string(7) "AvalidB"
