--TEST--
MustacheData exposes public object properties without leaking other property shapes
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
?>
--FILE--
<?php
class ParentPropertyContext {
  public $inherited = 'parent-public';
  private $collision = 'parent-private';
  protected $protectedValue = 'parent-protected';
  public static $staticValue = 'static';
  public string $uninitialized;
  public string $initialized = 'ready';
}

class ChildPropertyContext extends ParentPropertyContext {
  public $collision = 'child-public';
  private $privateValue = 'child-private';
}

$dynamic = new stdClass();
$dynamic->value = 'dynamic';

$mustache = new Mustache();
echo '<dynamic>', $mustache->render('{{value}}', $dynamic), "</dynamic>\n";

$template = "inherited={{inherited}}\n"
  . "collision={{collision}}\n"
  . "protected={{protectedValue}}\n"
  . "private={{privateValue}}\n"
  . "uninitialized={{uninitialized}}\n"
  . "initialized={{initialized}}\n"
  . 'static={{staticValue}}';

echo '<render>', $mustache->render($template, new ChildPropertyContext()), '</render>';
?>
--EXPECT--
<dynamic>dynamic</dynamic>
<render>inherited=parent-public
collision=child-public
protected=
private=
uninitialized=
initialized=ready
static=</render>
