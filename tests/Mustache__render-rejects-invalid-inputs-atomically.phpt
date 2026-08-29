--TEST--
Mustache::render() rejects invalid inputs without returning partial output
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
function expectError($label, $callback) {
  $warnings = array();
  set_error_handler(function ($severity, $message) use (&$warnings) {
    $warnings[] = $message;
    return true;
  });

  try {
    $result = $callback();
    echo $label, ' returned ', gettype($result), ' warnings=', count($warnings), "\n";
  } catch (Throwable $error) {
    echo $label, ' threw ', get_class($error), ' warnings=', count($warnings), "\n";
  } finally {
    restore_error_handler();
  }
}

$mustache = new Mustache();
$resource = fopen('php://memory', 'r');
$overBudgetAST = $mustache->parse(
  '{{' . implode('.', array_fill(0, 257, 'part')) . '}}'
);
$corruptTemplate = new MustacheTemplate('valid');
$templateProperty = new ReflectionProperty(MustacheTemplate::class, 'template');
if (PHP_VERSION_ID < 80100) {
  $templateProperty->setAccessible(true);
}
$templateProperty->setValue($corruptTemplate, array());

expectError('template', function () use ($mustache) {
  return $mustache->render(123, array());
});
expectError('corrupt template', function () use ($mustache, $corruptTemplate) {
  return $mustache->render($corruptTemplate, array());
});
expectError('data', function () use ($mustache, $resource) {
  return $mustache->render('unused', $resource);
});
expectError('nested data', function () use ($mustache, $resource) {
  return $mustache->render('unused', array('resource' => $resource));
});
expectError('partial key', function () use ($mustache) {
  return $mustache->render('A{{>x}}B', array(), array('unused'));
});
expectError('partial value', function () use ($mustache) {
  return $mustache->render('A{{>x}}B', array(), array('x' => 123));
});
expectError('corrupt partial template', function () use ($mustache, $corruptTemplate) {
  return $mustache->render('A{{>x}}B', array(), array('x' => $corruptTemplate));
});
expectError('empty partial template', function () use ($mustache) {
  return $mustache->render('A{{>x}}B', array(), array('x' => new MustacheTemplate()));
});
expectError('over-budget AST partial', function () use ($mustache, $overBudgetAST) {
  return $mustache->render('A{{>x}}B', array(), array('x' => $overBudgetAST));
});

fclose($resource);
var_dump($mustache->render('{{name}}', array('name' => 'Ada')));
?>
--EXPECT--
template threw TypeError warnings=0
corrupt template threw ValueError warnings=0
data threw TypeError warnings=0
nested data threw ValueError warnings=0
partial key threw ValueError warnings=0
partial value threw ValueError warnings=0
corrupt partial template threw ValueError warnings=0
empty partial template threw ValueError warnings=0
over-budget AST partial threw ValueError warnings=0
string(3) "Ada"
