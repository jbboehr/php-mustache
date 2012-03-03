<?php

class MustacheNativeRenderer
{
  static public function render($tree, array $data, array $partials = array())
  {
    $dataStack = array();
    $dataStackPos = 0;
    $dataStack[$dataStackPos] = &$data;
    
    return self::_renderNode($tree, $dataStack, $dataStackPos, $partials);
  }
  
  protected static function _renderNode($node, &$dataStack, $dataStackPos, &$partials)
  {
    $data = &$dataStack[$dataStackPos];
    $nstr = $node->data;
    
    $output = '';
    switch( $node->type ) {
      case MustacheNativeParser::NODE_ROOT:
        foreach( $node->children as $child ) {
          $output .= self::_renderNode($child, $dataStack, $dataStackPos, $partials);
        }
        break;
      case MustacheNativeParser::NODE_OUTPUT:
        if( empty($node->skip) ) {
          $output .= $nstr;
        }
        break;
      case MustacheNativeParser::NODE_SECTION:
        if( !empty($node->children) ) {
          if( empty($data[$nstr]) ) {
            if( $node->flags & MustacheNativeParser::FLAG_NEGATE ) {
              foreach( $node->children as $child ) {
                $output .= self::_renderNode($child, $dataStack, $dataStackPos, $partials);
              }
            }
          } else if( is_scalar($data[$nstr]) ) {
            foreach( $node->children as $child ) {
              $output .= self::_renderNode($child, $dataStack, $dataStackPos, $partials);
            }
          } else if( is_array($data[$nstr]) ) {
            if( isset($data[$nstr][0]) ) {
              // Numeric array
              foreach( $data[$nstr] as &$newData ) {
                $dataStack[$dataStackPos + 1] = &$newData;
                foreach( $node->children as $child ) {
                  $output .= self::_renderNode($child, $dataStack, $dataStackPos + 1, $partials);
                }
              }
              unset($dataStack[$dataStackPos + 1]);
            } else {
              // Associative array
              $dataStack[$dataStackPos + 1] = &$data[$nstr];
              foreach( $node->children as $child ) {
                $output .= self::_renderNode($child, $dataStack, $dataStackPos + 1, $partials);
              }
            }
          }
        }
        break;
      case MustacheNativeParser::NODE_TAG:
        if( $node->flags & MustacheNativeParser::FLAG_PARTIAL ) {
          if( !empty($partials[$nstr]) ) {
            $output .= self::_renderNode($partials[$nstr], $dataStack, $dataStackPos, $partials);
          }
        } else {
          $str = '';
          if( is_scalar($data) ) {
            if( $nstr == '.' ) {
              $str = $data;
            }
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
            
            if( is_scalar($ref) ) {
              $str = (string) $ref;
            }
          }
          if( $node->flags & MustacheNativeParser::FLAG_COMMENT ) {
            // Do nothing
          } else if( $node->flags & MustacheNativeParser::FLAG_ESCAPE ) {
            $output .= htmlspecialchars($str, ENT_QUOTES, 'UTF-8');
          } else {
            $output .= $str;
          }
        }
        break; 
      default:
        throw new Exception('Unknown node: ' . $node->name);
        break;
    }
    return $output;
  }
}
