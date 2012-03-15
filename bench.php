<?php

$count = 100000;

$tmplStr = '{{test}}';
$dataArr = array(
  'test' => 'val',
);

printf('%10s %10s %10s %10s' . PHP_EOL, 'Type', 'Total (s)', 'Each (s)', 'Count');

// Normal
$start = microtime(true);
$mustache = new Mustache();
for( $i = 0; $i < $count; $i++ ) {
  $out = $mustache->render($tmplStr, $dataArr);
}
$stop = microtime(true);
printf('%10s %10f %10f %10d' . PHP_EOL, 'Normal', ($stop - $start), (($stop - $start) / $count), $count);

// Compiled
$start = microtime(true);
$mustache = new Mustache();
$ret = $mustache->compile($tmplStr);
for( $i = 0; $i < $count; $i++ ) {
  $out = $ret->render($dataArr);
}
$stop = microtime(true);
printf('%10s %10f %10f %10d' . PHP_EOL, 'Compiled', ($stop - $start), (($stop - $start) / $count), $count);

// Compiled+Data
$start = microtime(true);
$tmpl = new MustacheTemplate($tmplStr);
$data = new MustacheData($dataArr);
for( $i = 0; $i < $count; $i++ ) {
  $out = $tmpl->render($data);
}
$stop = microtime(true);
printf('%10s %10f %10f %10d' . PHP_EOL, 'Compiled2', ($stop - $start), (($stop - $start) / $count), $count);

