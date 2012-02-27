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
        $tokens[] = array(
          'type' => 'tag',
          'data' => $tokenBuffer,
          'sectionStart' => $tagIsStartSection,
          'sectionStop' => $tagIsStopSection,
        );
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
      if( $v['type'] == 'tag' ) {
        if( $v['sectionStart'] ) {
          // Add to tree
          $stack[$depth]['children'][] = &$v; //$i;
          // Push stack
          $v['children'] = array();
          $depth++;
          $stack[$depth] = &$v;
          continue;
        } else if( $v['sectionStop'] ) {
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
    
    // @todo processing
    
  }
}

//$mustache = new MustacheNative();
//echo $mustache->render('test {{var}} test {{#aa}} aha! {{/aa}}', array('var' => 'val'));

