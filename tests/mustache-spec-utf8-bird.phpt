--TEST--
Bird
--DESCRIPTION--
Bird
--SKIPIF--
<?php if(!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$test = array (
  'name' => 'Bird',
  'desc' => 'Bird',
  'data' => 
  array (
    'animal' => '鳥',
  ),
  'template' => '{{animal}}は卵の中から抜け出ようと戦う。卵は世界だ。生まれようと欲する物は、一つの世界を破壊しなければならない。{{animal}}は神に向かって飛ぶ。神の名はＡｂｒａｘａｓと言う。',
  'expected' => '鳥は卵の中から抜け出ようと戦う。卵は世界だ。生まれようと欲する物は、一つの世界を破壊しなければならない。鳥は神に向かって飛ぶ。神の名はＡｂｒａｘａｓと言う。',
);
$mustache = new Mustache();
echo $mustache->render($test["template"], $test["data"]);
?>
--EXPECTREGEX--
鳥は卵の中から抜け出ようと戦う。卵は世界だ。生まれようと欲する物は、一つの世界を破壊しなければならない。鳥は神に向かって飛ぶ。神の名はＡｂｒａｘａｓと言う。