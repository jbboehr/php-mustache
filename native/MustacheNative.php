<?php

require 'MustacheNativeTokenizer.php';
require 'MustacheNativeParser.php';
require 'MustacheNativeCompiler.php';
require 'MustacheNativeRenderer.php';


class MustacheNative
{
  protected $_startSequence = '{{';
  protected $_stopSequence = '}}';
  protected $_escapeByDefault = true;
  protected $_errors;
  
  public function getEscapeByDefault()
  {
    return $this->_escapeByDefault;
  }
  
  public function getStartSequence()
  {
    return $this->_startSequence;
  }
  
  public function getStopSequence()
  {
    return $this->_stopSequence;
  }
  
  public function setEscapeByDefault($flag)
  {
    $this->_escapeByDefault = (bool) $flag;
    return $this;
  }
  
  public function setStartSequence($start)
  {
    $this->_startSequence = (string) $start;
    return $this;
  }
  
  public function setStopSequence($stop)
  {
    $this->_stopSequence = (string) $stop;
    return $this;
  }
  
  public function getErrors()
  {
    return $this->_errors;
  }
}




//$data = array();
//for( $i = 0; $i < 10000; $i++ ) {
//  $data[] = array(
//    'comment_id' => $i,
//    'body' => '<' . md5($i) . '>',
//  );
//}
//$data = array(
//  'comments' => array(
//    'test' => 'test',
//    'list' => $data,
//  ),
//);
//
//try {
//  $start = microtime(true);
//  
//  $tmpl = "{{#comments}} {{#list}} {{comment_id}} {{body}} {{/list}} {{test}} \n{{/comments}}";
//  $tokens = MustacheNativeTokenizer::tokenize($tmpl);
//  $tree = MustacheNativeParser::parse($tokens);
//  $output = MustacheNativeRenderer::render($tree, $data);
//  $stop = microtime(true);
//
//  //var_dump($output);
//  var_dump($stop - $start);
//} catch( Exception $e ) {
//  echo $e;
//}
//die();