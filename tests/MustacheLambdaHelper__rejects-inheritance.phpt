--TEST--
MustacheLambdaHelper rejects inheritance
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--INI--
fatal_error_backtraces=0
--FILE--
<?php
class ApplicationMustacheLambdaHelper extends MustacheLambdaHelper
{
}
echo "subclass accepted\n";
?>
--EXPECTF--
Fatal error: Class ApplicationMustacheLambdaHelper %r(?:cannot extend final class MustacheLambdaHelper|may not inherit from final class \(MustacheLambdaHelper\))%r in %s on line %d
