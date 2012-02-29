<?php

class MustacheNativeRenderer
{
  static public function render($tree, array $data)
  {
    $output = '';
    foreach( $tree->children as $node ) {
      $output .= self::_renderNode($node, $data);
    }
    return $output;
  }
  
  protected static function _renderNode($node, $data)
  {
    $output = '';
    switch( $node->type ) {
      case MustacheNativeParser::NODE_OUTPUT:
        $output .= $node->data;
        break;
      case MustacheNativeParser::NODE_SECTION:
        if( !empty($node->children) ) {
          if( empty($data[$node->data]) ) {
            // Do nothing
          } else if( is_scalar($data[$node->data]) ) {
            foreach( $node->children as $child ) {
              $output .= self::_renderNode($child, $data);
            }
          } else if( is_array($data[$node->data]) ) {
            if( isset($data[$node->data][0]) ) {
              // Numeric array
              foreach( $data[$node->data] as $newData ) {
                foreach( $node->children as $child ) {
                  $output .= self::_renderNode($child, $newData);
                }
              }
            } else {
              // Associative array
              foreach( $node->children as $child ) {
                $output .= self::_renderNode($child, $data[$node->data]);
              }
            }
          }
        }
        break;
      case MustacheNativeParser::NODE_TAG:
        if( $node->flags & MustacheNativeParser::FLAG_PARTIAL ) {
          throw new Exception('Partials not supported');
        }
        
        $str = '';
        if( is_scalar($data) ) {
          if( $node->data == '.' ) {
            $str = $data;
          }
        } else {
          $str = @$data[$node->data];
        }
        if( $node->flags & MustacheNativeParser::FLAG_COMMENT ) {
          // Do nothing
        } else if( $node->flags & MustacheNativeParser::FLAG_ESCAPE ) {
          $output .= htmlspecialchars($str, ENT_QUOTES, 'UTF-8');
        } else {
          $output .= $str;
        }
        break; 
      default:
        throw new Exception('Unknown node: ' . $node->name);
        break;
    }
    return $output;
  }
}
