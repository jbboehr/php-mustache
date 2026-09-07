--TEST--
MustacheData preserves sparse integer-key iteration order without changing the input
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$items = [8 => 'first', -2 => 'second', 3 => 'third'];
$data = new MustacheData(['items' => $items]);
$converted = $data->toValue();
echo 'converted: ', json_encode($converted['items']), "\n";

$mustache = new Mustache();
echo $mustache->render('{{#items}}[{{.}}]{{/items}}', $data), "\n";
echo 'original keys: ', json_encode(array_keys($items)), "\n";
echo 'original values: ', json_encode(array_values($items)), "\n";
?>
--EXPECT--
converted: ["first","second","third"]
[first][second][third]
original keys: [8,-2,3]
original values: ["first","second","third"]
