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
  
  public function render($tmpl, array $data)
  {
    $tokens = MustacheNativeTokenizer::tokenize($tmpl, $this->_startSequence, $this->_stopSequence);
    if( !$tokens ) {
      return false;
    }
    
    $tree = MustacheNativeParser::parse($tokens);
    if( !$tree ) {
      return false;
    }
    
    return MustacheNativeRenderer::render($tree, $data);
  }
  
  public function compile($tmpl)
  {
    $tokens = MustacheNativeTokenizer::tokenize($tmpl, $this->_startSequence, $this->_stopSequence);
    if( !$tokens ) {
      return false;
    }
    
    $tree = MustacheNativeParser::parse($tokens);
    if( !$tree ) {
      return false;
    }
    
    return MustacheNativeCompiler::compile($tree);
  }
  
  public function parse($tokens)
  {
    return MustacheNativeParser::parse($tokens);
  }
  
  public function tokenize($tmpl)
  {
    return MustacheNativeTokenizer::tokenize($tmpl, $this->_startSequence, $this->_stopSequence);
  }
  
  public function renderTree($tree, array $data)
  {
    return MustacheNativeRenderer::render($tree, $data);
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
//  $tmpl = "{{! this is a comment}} {{#comments}} {{#list}} {{comment_id}} {{body}} {{/list}} {{test}} \n{{/comments}}";
//  $tokens = MustacheNativeTokenizer::tokenize($tmpl);
//  $tree = MustacheNativeParser::parse($tokens);
//  $output = MustacheNativeRenderer::render($tree, $data);
////  $output = MustacheNativeCompiler::compile($tree);
//  $stop = microtime(true);
//  echo '<?php ' . $output;
//
//  //var_dump($output);
//  var_dump($stop - $start);
//} catch( Exception $e ) {
//  echo $e;
//}
//die();



//try {
//  $fn = require('test.php');
//  
//  $start = microtime(true);
//  
//  $output = $fn($data);
//  $stop = microtime(true);
//  var_dump($output);
//  //var_dump($output);
//  var_dump($stop - $start);
//} catch( Exception $e ) {
//  echo $e;
//}
//die();
