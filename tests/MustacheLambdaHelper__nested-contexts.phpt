--TEST--
MustacheLambdaHelper keeps nested callback contexts independent
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
function renderNestedLambda(Mustache $mustache, $template) {
  $outerHelper = null;
  $innerHelper = null;
  $data = array(
    'name' => 'Ada',
    'inner' => function ($text, MustacheLambdaHelper $helper) use (&$innerHelper) {
      $innerHelper = $helper;
      return $helper->render($text);
    },
    'outer' => function ($text, MustacheLambdaHelper $helper) use (&$outerHelper, &$innerHelper) {
      $outerHelper = $helper;
      $nested = $helper->render('{{#inner}}{{name}}{{/inner}}');

      try {
        $innerHelper->render('{{name}}');
        $innerState = 'active';
      } catch (MustacheException $e) {
        $innerState = 'inactive';
      }

      return $nested . ':' . $innerState . ':' . $helper->render('{{name}}');
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
renderNestedLambda($mustache, $source);
renderNestedLambda($mustache, $mustache->parse($source));
?>
--EXPECT--
string(16) "Ada:inactive:Ada"
string(41) "Lambda render context is no longer active"
string(41) "Lambda render context is no longer active"
string(16) "Ada:inactive:Ada"
string(41) "Lambda render context is no longer active"
string(41) "Lambda render context is no longer active"
