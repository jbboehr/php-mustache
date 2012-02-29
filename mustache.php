<?php

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
  
  public function compile($tmpl)
  {
    // Tokenize
    $tokens = $this->tokenize($tmpl);
    if( !$tokens ) {
      return false;
    }
    
    // Treeify
    $tree = $this->treeify($tokens);
    if( !$tree ) {
      return false;
    }
    
    $output = $this->_compileTree($tree);
    if( !$output ) {
      return false;
    }
    
    return $output;
  }
  
  public function renderCompiled($cmpl, array $data)
  {
    $fn = include $cmpl;
    return $fn($data);
  }
  
  public function render($tmpl, array $data)
  {
    // Tokenize
    $tokens = $this->tokenize($tmpl);
    if( !$tokens ) {
      return false;
    }
    
    // Treeify
    $tree = $this->treeify($tokens);
    if( !$tree ) {
      return false;
    }
    
    // Render
    $output = $this->renderTree($tree, $data);
    if( !$output ) {
      return false;
    }
    
    return $output;
  }
  
  public function renderTree($tree, $data)
  {
    if( true ) {
      $output = '';
      foreach( $tree['children'] as $token ) {
        $output .= $this->_renderToken($token, $data);
      }
    } else {
      $output = $this->_renderTree($tree, $data);
    }
    
    return $output;
  }
  
  public function tokenize($tmpl)
  {
    $tmpl .= ' '; // Hack
    $tmplL = strlen($tmpl);
    
    $startS = $this->_startSequence;
    $startC = $startS[0];
    $startL = strlen($startS);
    $stopS = $this->_stopSequence;
    $stopC = $stopS[0];
    $stopL = strlen($stopS);
    
    $outputBuffer = '';
    $tokenBuffer = '';
    
    $tokens = array();
    
    $inTag = false;
    $tagIsStartSection = false;
    $tagIsStopSection = false;
    $escaped = $this->_escapeByDefault;
    
    $line = 0;
    $char = 0;
    $tagStartInfo = array();
    
    for( $i = 0; $i < $tmplL; $i++ ) {
      $c = $tmpl[$i];
      
      // Track line/char
      if( $c == "\n" ) {
        $line++;
        $char = 0;
      } else {
        $char++;
      }
      
      // Start Tag
      if( !$inTag &&
          $c == $startC &&
          substr($tmpl, $i, $startL) == $startS ) {
        // Start tag
        $inTag = true;
        $tagStartInfo = array($line, $char);
        $i += $startL - 1; // this messes up stuff
        continue;
      }
      
      // Stop tag
      if( $inTag &&
          $c == $stopC &&
          substr($tmpl, $i, $stopL) == $stopS ) {
        // Stop tag
        $inTag = false;
        $tagStartInfo = array();
        $i += $stopL - 1; // this messes up stuff
        continue;
      }
      
      // Start Section
      if( $inTag && $c == '#' ) {
        $tagIsStartSection = true;
        continue;
      }
      
      // Stop Section
      if( $inTag && $c == '/' ) {
        $tagIsStopSection = true;
        continue;
      }
      
      // Escape
      if( $inTag && $c == '&' ) {
        $escaped = !$this->_escapeByDefault;
        continue;
      }
      
      // Whoops
      if( $inTag && $c == '{' ) {
        $this->_error('Unclosed tag, line: ' . $tagStartInfo[0] . ', char: ' . $tagStartInfo[1], E_USER_WARNING);
        return false;
      }
      
      // Static
      if( $inTag && $outputBuffer ) {
        $tokens[] = array(
          'type' => 'string',
          'data' => $outputBuffer,
        );
        $outputBuffer = '';
      }
      
      // Tag
      if( !$inTag && $tokenBuffer ) {
        if( $tagIsStartSection || $tagIsStopSection ) {
          $tokens[] = array(
            'type' => 'section',
            'data' => $tokenBuffer,
            'isStart' => $tagIsStartSection,
            'isStop' => $tagIsStopSection,
          );
        } else {
          $tokens[] = array(
            'type' => 'var',
            'data' => $tokenBuffer,
            'escaped' => $escaped,
          );
        }
        $tokenBuffer = '';
        $tagIsStartSection = false;
        $tagIsStopSection = false;
        $escaped = $this->_escapeByDefault;
      }
      
      if( $inTag ) {
        $tokenBuffer .= $c;
      } else {
        $outputBuffer .= $c;
      }
    }
    
    // Whoops
    if( $inTag || $tokenBuffer ) {
      $this->_error('Unclosed tag, line: ' . $tagStartInfo[0] . ', char: ' . $tagStartInfo[1], E_USER_WARNING);
      return false;
    }
    
    if( $outputBuffer ) {
        $tokens[] = array(
          'type' => 'string',
          'data' => $outputBuffer,
        );
        $outputBuffer = '';
    }
    
    return $tokens;
  }
  
  public function treeify($tokens)
  {
    // Treeify
    $tree = array(
      'type' => 'root',
      'children' => array(),
    );
    $depth = 0;
    $stack = array();
    $stack[$depth] = &$tree;
    
    foreach( array_keys($tokens) as $i ) {
      $v = &$tokens[$i];
      if( $v['type'] == 'section' ) {
        if( $v['isStart'] ) {
          // Add to tree
          $stack[$depth]['children'][] = &$v; //$i;
          // Push stack
          $v['children'] = array();
          $depth++;
          $stack[$depth] = &$v;
          continue;
        } else if( $v['isStop'] ) {
          if( $v['data'] != $stack[$depth]['data'] ) {
            $this->_error('Mismatched section');
            return false;
          }
          // Pop stack
          unset($stack[$depth]);
          $depth--;
          
          // Add to tree?
          $stack[$depth]['children'][] = &$v; //$i;
          continue;
        }
      }
      
      $stack[$depth]['children'][] = &$v; //$i;
    }
    
    if( $depth > 0 ) {
      $this->_error('Mismatched section', E_USER_WARNING);
      return false;
    }
    
    return $tree;
  }
  
  protected function _renderTree($tree, $data)
  {
    $output = '';
    $depth = 0;
    $stack = array();
    $stack[$depth] = array(
      'i' => 0,
      'v' => &$tree,
      'a' => &$data,
      'ai' => null,
    );
    do {
      $i = &$stack[$depth]['i'];
      $v = &$stack[$depth]['v'];
      $a = &$stack[$depth]['a'];
      $ai = &$stack[$depth]['ai'];
      
      if( !isset($v['children'][$i]) ) {
        if( null !== $ai ) {
          if( isset($a[$ai]) ) {
            $ai++;
            $i = 0;
          } else {
            unset($stack[$depth]);
            $depth--;
          }
        } else if( $depth > 0 ) {
          unset($stack[$depth]);
          $depth--;
          //$stack[$depth]['i']++;
          continue;
        } else {
          break;
        }
      } else {
        $c = &$v['children'][$i];
        $i++;
        if( $c['type'] == 'section' &&
            $c['isStart'] ) {
          $depth++;
          if( isset($a[$c['data']]) &&
              is_array($a[$c['data']]) ) {
            $ca = &$a[$c['data']];
            $cai = 0;
          } else {
            $ca = &$a;
            $cai = null;
          }
          $stack[$depth] = array(
            'i' => 0,
            'v' => &$c,
            'a' => &$ca,
            'ai' => $cai,
          );
        } else if( $c['type'] == 'string' ) {
          $output .= $c['data'];
        } else if( $c['type'] == 'var' ) {
          if( null == $ai ) {
            $output .= @$a[$c['data']];
          } else {
            $output .= @$a[$ai][$c['data']];
          }
        }
      }
    } while(1);
    return $output;
  }
  
  protected function _renderToken($token, array $data)
  {
    $output = '';
    if( $token['type'] == 'string' ) {
      $output .= $token['data'];
    } else if( $token['type'] == 'var' ) {
      if( $token['escaped'] ) {
        $output .= htmlspecialchars(@$data[$token['data']], ENT_QUOTES, 'UTF-8');
      } else {
        $output .= @$data[$token['data']];
      }
    } else if( $token['type'] == 'section' ) {
      if( $token['isStart'] && !empty($token['children']) ) {
        if( empty($data[$token['data']]) ) {
          // Do nothing
        } else if( is_scalar($data[$token['data']]) ) {
          foreach( $token['children'] as $child ) {
            $output .= $this->_renderToken($child, $data);
          }
        } else if( is_array($data[$token['data']]) ) {
          if( isset($data[$token['data']][0]) ) {
            foreach( $data[$token['data']] as $newData ) {
              foreach( $token['children'] as $child ) {
                $output .= $this->_renderToken($child, $newData);
              }
            }
          } else {
            foreach( $token['children'] as $child ) {
              $output .= $this->_renderToken($child, $data[$token['data']]);
            }
          }
        }
      }
    }
    return $output;
  }
  
  protected function _compileTree($tree)
  {
    $output = '';
    foreach( $tree['children'] as $token ) {
      $output .= $this->_compileToken($token, 1);
    }
    return 'return function(&$data) {' . PHP_EOL
      . '  $buf = "";' . PHP_EOL
      . $output . PHP_EOL
      . '  return $buf;' . PHP_EOL
      . '};';
  }
  
  protected function _compileToken($token, $depth = 0)
  {
    $tokenStr = var_export($token['data'], true);
    $pre = str_repeat(' ', $depth * 2);
    //$ref = '&';
    $ref = '';
    
    $output = '';
    if( $token['type'] == 'string' ) {
      $output .= $pre . '$buf .= ' . $tokenStr . ';' . PHP_EOL;
    } else if( $token['type'] == 'var' ) {
      if( $token['escaped'] ) {
        $output .= $pre . '$buf .= htmlspecialchars(@$data[' . $tokenStr . '], ENT_QUOTES, "UTF-8");' . PHP_EOL;
      } else {
        $output .= $pre . '$buf .= @$data[' . $tokenStr . '];' . PHP_EOL;
      }
    } else if( $token['type'] == 'section' ) {
      if( $token['isStart'] && !empty($token['children']) ) {
        $output .= $pre . '$fn = function(' . $ref . '$cdata, $isSection = true) {' . PHP_EOL;
        $output .= $pre . '  $buf = "";' . PHP_EOL;
        $output .= $pre . '  if( $isSection ) {' . PHP_EOL;
        $output .= $pre . '    $fdata = ' . $ref . '$cdata;' . PHP_EOL;
        $output .= $pre . '  } else {' . PHP_EOL;
        $output .= $pre . '    $fdata = array(' . $ref . '$cdata);' . PHP_EOL;
        $output .= $pre . '  }' . PHP_EOL;
        $output .= $pre . '  foreach( $fdata as ' . $ref . '$data ) {' . PHP_EOL;
        foreach( $token['children'] as $child ) {
          $output .= $this->_compileToken($child, $depth + 2);
        }
        $output .= $pre . '  }' . PHP_EOL;
        $output .= $pre . '  return $buf;' . PHP_EOL;
        $output .= $pre . '};' . PHP_EOL;
        
        $output .= $pre . 'if( empty($data[' . $tokenStr . ']) ) {' . PHP_EOL;
        $output .= $pre . '  // Do nothing' . PHP_EOL;
        $output .= $pre . '} else if( is_scalar($data[' . $tokenStr . ']) ) {' . PHP_EOL;
        $output .= $pre . '  $buf .= $fn($data, false);' . PHP_EOL;
        $output .= $pre . '} else if( is_array($data[' . $tokenStr . ']) ) {' . PHP_EOL;
        $output .= $pre . '  if( isset($data[' . $tokenStr . '][0]) ) {' . PHP_EOL;
        $output .= $pre . '    $buf .= $fn($data[' . $tokenStr . ']);' . PHP_EOL;
        $output .= $pre . '  } else {' . PHP_EOL;
        $output .= $pre . '    $buf .= $fn($data[' . $tokenStr . '], false);' . PHP_EOL;
        $output .= $pre . '  }' . PHP_EOL;
        $output .= $pre . '}' . PHP_EOL;
      }
    }
    return $output;
  }
  
  protected function _error($message, $level = E_USER_NOTICE)
  {
    trigger_error($message, $level);
    $this->_errors[] = $message;
    return $this;
  }
}



//error_reporting(E_ALL);



//$mustache = new MustacheNative();
//$cmpl = $mustache->compile("{{#comments}} {{comment_id}} {{body}} \n{{/comments}}");
//echo '<?php' . PHP_EOL . $cmpl;



//$data = array();
//for( $i = 0; $i < 10000; $i++ ) {
//  $data[] = array(
//    'comment_id' => $i,
//    'body' => '<' . md5($i) . '>',
//  );
//}
//$start = microtime(true);
//$mustache = new MustacheNative();
//$ret = $mustache->render("{{#comments}} {{#list}} {{comment_id}} {{body}} {{/list}} {{test}} \n{{/comments}}", array(
//  'comments' => array(
//    'test' => 'test',
//    'list' => $data,
//  ),
//));
//if( ($err = $mustache->getErrors()) ) {
//  var_dump($err);
//}
//$stop = microtime(true);
//echo $ret;
//var_dump($stop - $start);
//var_dump(memory_get_peak_usage());



//$data = array();
//for( $i = 0; $i < 10000; $i++ ) {
//  $data[] = array(
//    'comment_id' => $i,
//    'body' => md5($i),
//  );
//}
//$start = microtime(true);
//$mustache = new MustacheNative();
//$ret = $mustache->renderCompiled("./test.php", array(
//  'comments' => $data,
//));
//$stop = microtime(true);
//echo $ret;
//var_dump($stop - $start);
//var_dump(memory_get_peak_usage());
