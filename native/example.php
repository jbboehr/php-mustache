<?php

require 'MustacheNative.php';

$tmpl = '{{^boolean}}This should not be rendered.{{/boolean}}';
$mustache = new MustacheNative();
$data = array(
  'boolean' => true
);
var_dump($mustache->render($tmpl, $data));
die();

$data = array();
$count = 1000;
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
  $start = microtime(true);
  
  $tmpl = "{{! this is a comment}} {{#comments}} {{#list}} {{comment_id}}  {{body}} {{/list}} {{test}} \n{{/comments}}";
  $tokens = MustacheNativeTokenizer::tokenize($tmpl);
  $tree = MustacheNativeParser::parse($tokens);
  $output = MustacheNativeRenderer::render($tree, $data);
//  $output = MustacheNativeCompiler::compile($tree);
  $stop = microtime(true);
  echo $output;

  var_dump($stop - $start);
} catch( Exception $e ) {
  echo $e;
}
die();