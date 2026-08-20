--TEST--
MustacheLambdaHelper restores the outer context after a nested callback exception
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
function renderNestedLambdaFailure(Mustache $mustache, $template) {
  $outerHelper = null;
  $innerHelper = null;
  $data = array(
    'name' => 'Ada',
    'inner' => function ($text, MustacheLambdaHelper $helper) use (&$innerHelper) {
      $innerHelper = $helper;
      throw new RuntimeException('inner failed');
    },
    'outer' => function ($text, MustacheLambdaHelper $helper) use (&$outerHelper, &$innerHelper) {
      $outerHelper = $helper;

      try {
        $helper->render('{{#inner}}ignored{{/inner}}');
        $message = 'inner did not throw';
      } catch (RuntimeException $e) {
        $message = $e->getMessage();
      }

      try {
        $innerHelper->render('{{name}}');
        $innerState = 'active';
      } catch (MustacheException $e) {
        $innerState = 'inactive';
      }

      return $message . ':' . $innerState . ':' . $helper->render('{{name}}');
    },
  );

  var_dump($mustache->render($template, $data));
  foreach (array($outerHelper, $innerHelper) as $helper) {
    try {
      $helper->render('{{name}}');
      echo "retained helper remained active\n";
    } catch (MustacheException $e) {
      var_dump($e->getMessage());
    }
  }
}

$mustache = new Mustache();
$source = '{{#outer}}ignored{{/outer}}';
renderNestedLambdaFailure($mustache, $source);
renderNestedLambdaFailure($mustache, $mustache->parse($source));
?>
--EXPECT--
string(25) "inner failed:inactive:Ada"
string(41) "Lambda render context is no longer active"
string(41) "Lambda render context is no longer active"
string(25) "inner failed:inactive:Ada"
string(41) "Lambda render context is no longer active"
string(41) "Lambda render context is no longer active"
