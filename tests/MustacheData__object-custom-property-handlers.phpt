--TEST--
MustacheData exposes declared properties without exposing custom-handler storage
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
?>
--FILE--
<?php
class ArrayObjectPropertyContext extends ArrayObject {
  public $declared = 'visible';
  protected $hidden = 'hidden';
}

$context = new ArrayObjectPropertyContext(
  array('offset' => 'stored'),
  ArrayObject::ARRAY_AS_PROPS
);
$template = "declared={{declared}}\noffset={{offset}}\nhidden={{hidden}}";

echo '<render>', (new Mustache())->render($template, $context), '</render>';
?>
--EXPECT--
<render>declared=visible
offset=
hidden=</render>
