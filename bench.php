<?php

$count = 100000;
$startMemory = memory_get_usage(true);

$tmplStr = '{{test}}';
$dataArr = array(
  'test' => 'val',
);
//$tmplStr = '{{#test}}{{var}}{{/test}}';
//$dataArr = array(
//  'test' => array(
//    'val1',
//    'val2',
//    'val3',
//    'val4',
//    'val5',
//  ),
//);

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

// Parsed
$name = 'Parsed';
$startMemory = memory_get_usage(true);
$start = microtime(true);
$mustache = new Mustache();
$ret = $mustache->parse($tmplStr);
for( $i = 0; $i < $count; $i++ ) {
  $out = $mustache->render($ret, $dataArr);
}
$stop = microtime(true);
$stopMemory = memory_get_usage(true);
printf('%10s %10f %10f %10d %10s' . PHP_EOL, 
    $name, ($stop - $start), (($stop - $start) / $count), $count, number_format($stopMemory - $startMemory));

// Parsed+Data
$name = 'Parsed2';
$startMemory = memory_get_usage(true);
$start = microtime(true);
$tmpl = $mustache->parse($tmplStr);
$data = new MustacheData($dataArr);
for( $i = 0; $i < $count; $i++ ) {
  $out = $mustache->render($tmpl, $data);
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
  $out = $mustache->execute($ret, $dataArr);
}
$stop = microtime(true);
$stopMemory = memory_get_usage(true);
printf('%10s %10f %10f %10d %10s' . PHP_EOL, 
    $name, ($stop - $start), (($stop - $start) / $count), $count, number_format($stopMemory - $startMemory));

// Compiled+Data
$name = 'Compiled2';
$startMemory = memory_get_usage(true);
$start = microtime(true);
$tmpl = $mustache->compile($tmplStr);
$data = new MustacheData($dataArr);
for( $i = 0; $i < $count; $i++ ) {
  $out = $mustache->execute($tmpl, $data);
}
$stop = microtime(true);
$stopMemory = memory_get_usage(true);
printf('%10s %10f %10f %10d %10s' . PHP_EOL, 
    $name, ($stop - $start), (($stop - $start) / $count), $count, number_format($stopMemory - $startMemory));

echo PHP_EOL;
printf("Memory (start): %s" . PHP_EOL, number_format($startMemory));
printf("Memory (end): %s" . PHP_EOL, number_format(memory_get_usage(true)));
printf("Memory (peak): %s" . PHP_EOL, number_format(memory_get_peak_usage(true)));
