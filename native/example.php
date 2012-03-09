<?php

require 'MustacheNative.php';

// Configuration
$class = 'MustacheNative';
$count = 10000;
$silent = true;

for( $i = 1; $i < $argc; $i++ ) {
  switch( strtolower($argv[$i]) ) {
    case 'php': case 'native':
      $class = 'MustacheNative';
      break;
    case 'c': case 'c++': case 'cpp':
      $class = 'Mustache';
      break;
    case 'echo': case 'output': case 'verbose':
      $silent = false;
      break;
  }
  if( is_numeric($argv[$i]) ) {
    $count = (int) $argv[$i];
  }
}

// Generate data
$data = array();
for( $i = 0; $i < $count; $i++ ) {
  $data[] = array(
    'comment_id' => $i,
    'body' => '<' . md5($i) . '>',
  );
}
$data = array(
  'comments' => array(
    'test' => 'test',
    'list' => $data,
  ),
);

try {
  $tmpl = <<<EOF
{{! this is a comment}}
{{#comments}}
  {{#list}}
    {{comment_id}}
    {{body}}
  {{/list}}
{{test}}
{{/comments}}
EOF;
  $mustache = new $class();
  
  $start = microtime(true);
  $output = $mustache->render($tmpl, $data);
  $stop = microtime(true);
  
  if( !$silent ) {
    echo $output;
  }
  
  printf("Language: %s" . PHP_EOL, $class == 'Mustache' ? 'C++' : 'PHP' );
  printf("Class: %s" . PHP_EOL, $class);
  printf("Count: %d" . PHP_EOL, $count);
  printf("Time (total): %f" . PHP_EOL, $stop - $start);
  printf("Time (each): %f" . PHP_EOL, ($stop - $start) / $count);
} catch( Exception $e ) {
  echo $e;
}
die();
