--TEST--
Mustache::render() converts and releases successful lambda return values
--SKIPIF--
<?php
if (!extension_loaded('mustache') || !class_exists('WeakReference')) die('skip ');
?>
--FILE--
<?php
class StringableLambdaResult {
  public static $destructions = 0;
  private $value;

  public function __construct($value) {
    $this->value = $value;
  }

  public function __toString() {
    return $this->value;
  }

  public function __destruct() {
    self::$destructions++;
  }
}

$mustache = new Mustache();
$weakHelper = null;
$data = array(
  'name' => 'Ada',
  'null' => function () { return null; },
  'false' => function () { return false; },
  'true' => function () { return true; },
  'integer' => function () { return 42; },
  'float' => function () { return 1.5; },
  'string' => function () { return "a\0b"; },
  'object' => function () { return new StringableLambdaResult('{{name}}'); },
  'sectionObject' => function ($text, MustacheLambdaHelper $helper) use (&$weakHelper) {
    $weakHelper = WeakReference::create($helper);
    return new StringableLambdaResult($helper->render($text));
  },
);

$output = $mustache->render(
  '{{null}}|{{false}}|{{true}}|{{integer}}|{{float}}|{{{string}}}|{{object}}',
  $data
);
var_dump(bin2hex($output));
var_dump($mustache->render('{{#sectionObject}}{{name}}{{/sectionObject}}', $data));
gc_collect_cycles();
var_dump($weakHelper->get());
var_dump(StringableLambdaResult::$destructions);
?>
--EXPECT--
string(36) "7c7c317c34327c312e357c6100627c416461"
string(3) "Ada"
NULL
int(2)
