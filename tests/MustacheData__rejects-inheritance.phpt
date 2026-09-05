--TEST--
MustacheData rejects inheritance
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--INI--
fatal_error_backtraces=0
--FILE--
<?php
class ApplicationMustacheData extends MustacheData
{
}
echo "subclass accepted\n";
?>
--EXPECTF--
Fatal error: Class ApplicationMustacheData %r(?:cannot extend final class MustacheData|may not inherit from final class \(MustacheData\))%r in %s on line %d
