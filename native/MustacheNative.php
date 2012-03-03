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
  
  public function render($tmpl, array $data, array $partials = array())
  {
    $tokens = MustacheNativeTokenizer::tokenize($tmpl, $this->_startSequence, $this->_stopSequence);
    if( !$tokens ) {
      return false;
    }
    
    $tree = MustacheNativeParser::parse($tokens);
    if( !$tree ) {
      return false;
    }
    
    if( null !== $partials ) {
      foreach( $partials as $name => &$partial ) {
        $ptokens = MustacheNativeTokenizer::tokenize($partial, $this->_startSequence, $this->_stopSequence);
        if( !$ptokens ) {
          $partial = false;
          continue;
        }

        $ptree = MustacheNativeParser::parse($ptokens);
        if( !$ptree ) {
          $partial = false;
          continue;
        }
        
        $partial = $ptree;
      }
    }
    
    return MustacheNativeRenderer::render($tree, $data, $partials);
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
