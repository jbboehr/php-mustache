<?php

class MustacheNative
{
  private $_startSequence = '{{';
  private $_stopSequence = '}}';
  
  public function getStartSequence()
  {
    return $this->_startSequence;
  }
  
  public function getStopSequence()
  {
    return $this->_stopSequence;
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
  
  public function render($tmpl, array $data)
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
    for( $i = 0; $i < $tmplL; $i++ ) {
      $c = $tmpl[$i];
      // Start Tag
      if( !$inTag &&
          $c == $startC &&
          substr($tmpl, $i, $startL) == $startS ) {
        // Start tag
        $inTag = true;
        $i += $startL - 1;
        continue;
      }
      
      // Stop tag
      if( $inTag &&
          $c == $stopC &&
          substr($tmpl, $i, $stopL) == $stopS ) {
        // Stop tag
        $inTag = false;
        $i += $stopL - 1;
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
          );
        }
        $tokenBuffer = '';
        $tagIsStartSection = false;
        $tagIsStopSection = false;
      }
      
      if( $inTag ) {
        $tokenBuffer .= $c;
      } else {
        $outputBuffer .= $c;
      }
    }
    
    // Whoops
    if( $inTag || $tokenBuffer ) {
      trigger_error('Unclosed tag');
      return false;
    }
    
    if( $outputBuffer ) {
        $tokens[] = array(
          'type' => 'string',
          'data' => $outputBuffer,
        );
        $outputBuffer = '';
    }
    
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
            trigger_error('Mismatched section');
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
      trigger_error('Mismatched section');
    }
    
    
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
  
  protected function _renderTree($tree, $data)
  {
    $depth = 0;
    $stack = array();
    $stack[$depth] = array(
      'i' => 0,
      'v' => &$tree,
    );
    do {
      $i = &$stack[$depth]['i'];
      $v = &$stack[$depth]['v'];
      if( !isset($v['children'][$i]) ) {
        if( $depth > 0 ) {
          unset($stack[$depth]);
          $depth--;
          //$stack[$depth]['i']++;
        } else {
          break;
        }
      } else {
        $c = $v['children'][$i];
        $i++;
        if( $c['type'] == 'section' &&
            $c['isStart'] ) {
          $depth++;
          $stack[$depth] = array(
            'i' => 0,
            'v' => &$c,
          );
        }
      }
    } while(1);
  }
  
  protected function _renderToken($token, array $data)
  {
    $output = '';
    if( $token['type'] == 'string' ) {
      $output .= $token['data'];
    } else if( $token['type'] == 'var' ) {
      $output .= @$data[$token['data']];
    } else if( $token['type'] == 'section' ) {
      if( $token['isStart'] ) {
        if( isset($data[$token['data']]) &&
            is_array($data[$token['data']]) ) {
          if( !empty($token['children']) ) {
            foreach( $data[$token['data']] as $newData ) {
              foreach( $token['children'] as $child ) {
                $output .= $this->_renderToken($child, $newData);
              }
            }
          }
        } else {
          if( !empty($data[$token['data']]) ) {
            if( !empty($token['children']) ) {
              foreach( $token['children'] as $child ) {
                $output .= $this->_renderToken($child, $data);
              }
            }
          }
        }
      }
    }
    return $output;
  }
}


//$data = array();
//for( $i = 0; $i < 10000; $i++ ) {
//  $data[] = array(
//    'comment_id' => $i,
//    'body' => md5($i),
//  );
//}
//$start = microtime(true);
//$mustache = new MustacheNative();
//echo $mustache->render('{{#comments}} {{comment_id}} {{body}} {{/comments}}', array(
//  'comments' => $data,
//));
//$stop = microtime(true);
//var_dump($stop - $start);
//var_dump(memory_get_peak_usage());
