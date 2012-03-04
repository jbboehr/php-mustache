<?php

//require 'MustacheNativeCompiler.php';

class MustacheNative
{
  const NODE_ROOT = 1;
  const NODE_OUTPUT = 2;
  const NODE_TAG = 3;
  
  const FLAG_NONE = 0;
  const FLAG_ESCAPE = 1;
  const FLAG_NEGATE = 2;
  const FLAG_SECTION = 4;
  const FLAG_STOP = 8;
  const FLAG_COMMENT = 16;
  const FLAG_PARTIAL = 32;
  const FLAG_INLINE_PARTIAL = 64;
  
  protected $_startSequence = '{{';
  protected $_stopSequence = '}}';
  protected $_escapeByDefault = true;
  
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
  
  public function render($tmpl, array $data, array $partials = array())
  {
    // Tokenize partials
    if( null !== $partials ) {
      foreach( $partials as $name => &$partial ) {
        $partial = $this->tokenize($partial);
      }
    }
    
    // Render
    return $this->renderTree($this->tokenize($tmpl), $data, $partials);
  }
  
  public function tokenize($tmpl)
  {
    $start = '{{';
    $stop = '}}';
    
    $tmplL = strlen($tmpl);
    $startC = $start[0];
    $startL = strlen($start);
    $stopC = $stop[0];
    $stopL = strlen($stop);
    
    $canHaveChildren = self::FLAG_SECTION | self::FLAG_NEGATE | self::FLAG_INLINE_PARTIAL;
    
    $pos = 0;
    $char = null;
    $skipUntil = null;
    $lineNo = 0;
    $charNo = 0;
    
    $inTag = false;
    $inTripleTag = false;
    
    $buffer = '';
    $tree = new stdClass;
    $tree->type = self::NODE_ROOT;
    $tree->data = '';
    $tree->flags = 0;
    $tree->children = array();
    
    $depth = 0;
    $stack = array();
    $stack[$depth] = $tree;
    
    // Scan loop
    for( $pos = 0; $pos < $tmplL; $pos++ ) {
      // Get current character
      $char = $tmpl[$pos];
      
      // Track line numbers
      if( $char == "\n" ) {
        $lineNo++;
        $charNo = 0;
      } else {
        $charNo++;
      }
      
      // Skip until position
      if( null !== $skipUntil ) {
        if( $pos <= $skipUntil ) {
          continue;
        } else {
          $skipUntil = null;
        }
      }
      
      // Switch char
      switch( $char ) {
        case $startC: // START TAG
          if( !$inTag &&
              substr($tmpl, $pos, $startL) == $start ) {
            // Close previous buffer
            if( $buffer ) {
              $node = new stdClass;
              $node->type = self::NODE_OUTPUT;
              $node->data = $buffer;
              $stack[$depth]->children[] = $node;
              $buffer = '';
            }
            // Open new buffer
            $inTag = true;
            $skipUntil = $pos + $startL - 1;
            // Triple mustache
            if( $start == '{{' && $tmpl[$pos+2] == '{' ) {
              $inTripleTag = true;
              $skipUntil++;
            }
          }
          break;
        case $stopC: // STOP TAG
          if( $inTag &&
              substr($tmpl, $pos, $stopL) == $stop ) {
            // Close previous buffer
            if( '' != ($buffer = trim($buffer)) ) {
              // Process tag buffer
              $flags = 0;
              switch( $buffer[0] ) {
                case '&':
                  $flags = self::FLAG_ESCAPE;
                  break;
                case '^':
                  $flags = self::FLAG_NEGATE;
                  break;
                case '#':
                  $flags = self::FLAG_SECTION;
                  break;
                case '/':
                  $flags = self::FLAG_STOP;
                  break;
                case '!':
                  $flags = self::FLAG_COMMENT;
                  break;
                case '>':
                  $flags = self::FLAG_PARTIAL;
                  break;
                case '<':
                  $flags = self::FLAG_INLINE_PARTIAL;
                  break;
              }
              if( $flags ) {
                $buffer = trim(substr($buffer, 1));
              }
              if( $inTripleTag ) {
                $flags |= self::FLAG_ESCAPE;
              }
              // Create node
              $node = new stdClass;
              $node->type = self::NODE_TAG;
              $node->data = $buffer;
              $node->flags = $flags;
              $stack[$depth]->children[] = $node;
              // Push/pop stack
              if( $flags & $canHaveChildren ) {
                $depth++;
                $stack[$depth] = $node;
              } else if( $flags & self::FLAG_STOP ) {
                unset($stack[$depth]);
                $depth--;
              }
              // Clear buffer
              $buffer = '';
            } else {
              self::_errorWithLocation('Empty buffer', $lineNo, $charNo);
            }
            // Open new buffer
            $inTag = false;
            $skipUntil = $pos + $stopL - 1;
            // Triple mustache
            if( $stop == '}}' && $inTripleTag ) {
              if( $tmpl[$pos+2] != '}' ) {
                self::_errorWithLocation('Missing closing triple mustache delimiter: ' . $char, $lineNo, $charNo);
              }
              $skipUntil++;
            }
            $inTripleTag = false;
          }
          break;
        default:
          break;
      }
      
      // Append to buffer
      if( null === $skipUntil ) {
        $buffer .= $char;
      }
    }
    
    if( $inTag ) {
      self::_errorWithLocation('Unclosed tag', $lineNo, $charNo);
    } else if( $buffer ) {
      $node = new stdClass;
      $node->type = self::NODE_OUTPUT;
      $node->data = $buffer;
      $stack[$depth]->children[] = $node;
    }
    
    return $tree;
  }
  
  public function renderTree($tree, array $data, array $partials = array())
  {
    $this->tree = $tree;
    $dataStack = array();
    $dataStackPos = 0;
    $dataStack[$dataStackPos] = &$data;
    
    return $this->_renderNode($tree, $dataStack, $dataStackPos, $partials);
  }
  
  protected function _renderNode($node, &$dataStack, $dataStackPos, &$partials)
  {
    $data = &$dataStack[$dataStackPos];
    if( !is_object($node) ) {
      var_dump($this->tree);
      var_dump($node);die();
    }
    $nstr = $node->data;
    $output = '';
    
    // Handle simple cases
    if( $node->type == self::NODE_ROOT ) {
      foreach( $node->children as $child ) {
        $output .= $this->_renderNode($child, $dataStack, $dataStackPos, $partials);
      }
      return $output;
    } else if( $node->type == self::NODE_OUTPUT ) {
      return $nstr;
    }
    
    // Resolve data
    $val = null;
    if( is_scalar($data) ) {
      if( $nstr == '.' ) {
        $val = $data;
      }
    } else if( isset($data[$nstr]) ) {
      $val = $data[$nstr];
    } else {
      // Dot notation
      $initial = $nstr;
      $parts = null;
      if( false !== strpos($nstr, '.') ) {
        $parts = explode('.', $nstr);
        $initial = $parts[0];
      }

      // Resolve up the data stack
      $ref = null;
      for( $i = $dataStackPos; $i >= 0; $i-- ) {
        if( isset($dataStack[$i][$initial]) ) {
          //if( is_scalar($dataStack[$i][$node->data]) ) {
            $ref = $dataStack[$i][$initial];
            break;
          //}
        }
      }

      // Resolve or dot notation
      $str = '';
      if( null !== $parts ) {
        // Do notation
        for( $i = 1, $l = count($parts); $i < $l; $i++ ) {
          if( is_array($ref) && isset($ref[$parts[$i]]) ) {
            $ref = $ref[$parts[$i]];
          } else {
            break;
          }
        }
      }

      $val = $ref;
    }
    
    switch( $node->flags ) {
      case self::FLAG_COMMENT:
      case self::FLAG_STOP:
      case self::FLAG_INLINE_PARTIAL:
        // Do nothing
        break;
      case self::FLAG_NEGATE:
      case self::FLAG_SECTION:
        // Section
        if( (bool) ($node->flags & self::FLAG_NEGATE) == !empty($val) ) {
          continue;
        }
        if( !empty($node->children) ) {
          if( empty($val) ) {
            foreach( $node->children as $child ) {
              $output .= $this->_renderNode($child, $dataStack, $dataStackPos, $partials);
            }
          } else if( is_scalar($val) ) {
            foreach( $node->children as $child ) {
              $output .= $this->_renderNode($child, $dataStack, $dataStackPos, $partials);
            }
          } else if( is_array($val) ) {
            if( isset($val[0]) ) {
              // Numeric array
              foreach( $val as &$newData ) {
                $dataStack[$dataStackPos + 1] = &$newData;
                foreach( $node->children as $child ) {
                  $output .= $this->_renderNode($child, $dataStack, $dataStackPos + 1, $partials);
                }
              }
              unset($dataStack[$dataStackPos + 1]);
            } else {
              // Associative array
              $dataStack[$dataStackPos + 1] = &$val;
              foreach( $node->children as $child ) {
                $output .= $this->_renderNode($child, $dataStack, $dataStackPos + 1, $partials);
              }
            }
          }
        }
        break;
      case self::FLAG_PARTIAL:
        // Partial
        if( !empty($partials[$nstr]) ) {
          $output .= $this->_renderNode($partials[$nstr], $dataStack, $dataStackPos, $partials);
        }
        break;
      case self::FLAG_ESCAPE:
      case self::FLAG_NONE:
        // Normal tag
        if( is_scalar($val) ) {
          if( (bool) ($node->flags & self::FLAG_ESCAPE) != $this->_escapeByDefault ) {
            $output .= htmlspecialchars($val, ENT_QUOTES, 'UTF-8');
          } else {
            $output .= $val;
          }
        } else {
          // error?
        }
        break;
      default:
        throw new Exception('Unknown node flags: ' . $node->flags);
        break;
    }
    
    return $output;
  }
  
  protected static function _errorWithLocation($msg, $line, $char)
  {
    $msg .= ' [Line: ' . $line . ', Char: ' . $char . ']';
    throw new Exception($msg);
  }
}
