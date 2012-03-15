<?php

$count = 100000;
$startMemory = memory_get_usage(true);

$tmplStr = '{{test}}';
$dataArr = array(
  'test' => 'val',
);

printf('%10s %10s %10s %10s %10s' . PHP_EOL, 'Type', 'Total (s)', 'Each (s)', 'Count', 'Memory Delta (B)');

// Normal
$name = 'Normal';
$startMemory = memory_get_usage(true);
$start = microtime(true);
$mustache = new Mustache();
for( $i = 0; $i < $count; $i++ ) {
  $out = $mustache->render($tmplStr, $dataArr);
}
$stop = microtime(true);
$stopMemory = memory_get_usage(true);
printf('%10s %10f %10f %10d %10s' . PHP_EOL, 
    $name, ($stop - $start), (($stop - $start) / $count), $count, number_format($stopMemory - $startMemory));

// Compiled
$name = 'Compiled';
$startMemory = memory_get_usage(true);
$start = microtime(true);
$mustache = new Mustache();
$ret = $mustache->compile($tmplStr);
for( $i = 0; $i < $count; $i++ ) {
  $out = $ret->render($dataArr);
}
$stop = microtime(true);
$stopMemory = memory_get_usage(true);
printf('%10s %10f %10f %10d %10s' . PHP_EOL, 
    $name, ($stop - $start), (($stop - $start) / $count), $count, number_format($stopMemory - $startMemory));

// Compiled+Data
$name = 'Compiled2';
$startMemory = memory_get_usage(true);
$start = microtime(true);
$tmpl = new MustacheTemplate($tmplStr);
$data = new MustacheData($dataArr);
for( $i = 0; $i < $count; $i++ ) {
  $out = $tmpl->render($data);
}
$stop = microtime(true);
$stopMemory = memory_get_usage(true);
printf('%10s %10f %10f %10d %10s' . PHP_EOL, 
    $name, ($stop - $start), (($stop - $start) / $count), $count, number_format($stopMemory - $startMemory));

echo PHP_EOL;
printf("Memory (start): %s" . PHP_EOL, number_format($startMemory));
printf("Memory (end): %s" . PHP_EOL, number_format(memory_get_usage(true)));
printf("Memory (peak): %s" . PHP_EOL, number_format(memory_get_peak_usage(true)));
