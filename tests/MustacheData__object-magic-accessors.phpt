--TEST--
MustacheData does not invoke magic accessors while collecting object properties
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
?>
--FILE--
<?php
class MagicPropertyContext {
  public $declared = 'visible';
  public $getCalls = 0;
  public $issetCalls = 0;
  public $debugCalls = 0;

  public function __get($name) {
    ++$this->getCalls;
    return 'magic-' . $name;
  }

  public function __isset($name) {
    ++$this->issetCalls;
    return true;
  }

  public function __debugInfo() {
    ++$this->debugCalls;
    return array('virtual' => 'debug');
  }
}

$context = new MagicPropertyContext();
$template = "declared={{declared}}\nvirtual={{virtual}}";

echo '<render>', (new Mustache())->render($template, $context), "</render>\n";
echo 'get=', $context->getCalls, "\n";
echo 'isset=', $context->issetCalls, "\n";
echo 'debug=', $context->debugCalls;
?>
--EXPECT--
<render>declared=visible
virtual=</render>
get=0
isset=0
debug=0
