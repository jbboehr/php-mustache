<?php

class MustacheNativeCompiler
{
  static protected $_ref = '';
  
  static public function compile($tree)
  {
    $output = '';
    foreach( $tree->children as $node ) {
      $output .= self::_compileNode($node, 1);
    }
    return 'return function(' . self::$_ref . '$data) {' . PHP_EOL
      . '  $buf = "";' . PHP_EOL
      . $output . PHP_EOL
      . '  return $buf;' . PHP_EOL
      . '};';
  }
  
  static protected function _compileNode($node, $depth = 0)
  {
    $tokenStr = var_export($node->data, true);
    $pre = str_repeat(' ', $depth * 2);
    $ref = self::$_ref;
    
    $output = '';
    switch( $node->type ) {
      case MustacheNativeParser::NODE_OUTPUT:
        $output .= $pre . '$buf .= ' . $tokenStr . ';' . PHP_EOL;
        break;
      case MustacheNativeParser::NODE_TAG:
        if( $node->flags & MustacheNativeParser::FLAG_ESCAPE ) {
          $output .= $pre . '$buf .= htmlspecialchars(@$data[' . $tokenStr . '], ENT_QUOTES, "UTF-8");' . PHP_EOL;
        } else {
          $output .= $pre . '$buf .= @$data[' . $tokenStr . '];' . PHP_EOL;
        }
        break;
      case MustacheNativeParser::NODE_SECTION:
        if( !empty($node->children) ) {
          $output .= $pre . '$fn = function(' . $ref . '$cdata, $isSection = true) {' . PHP_EOL;
          $output .= $pre . '  $buf = "";' . PHP_EOL;
          $output .= $pre . '  if( $isSection ) {' . PHP_EOL;
          $output .= $pre . '    $fdata = ' . $ref . '$cdata;' . PHP_EOL;
          $output .= $pre . '  } else {' . PHP_EOL;
          $output .= $pre . '    $fdata = array(' . $ref . '$cdata);' . PHP_EOL;
          $output .= $pre . '  }' . PHP_EOL;
          $output .= $pre . '  foreach( $fdata as ' . $ref . '$data ) {' . PHP_EOL;
          foreach( $node->children as $child ) {
            $output .= self::_compileNode($child, $depth + 2);
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
        break;
    }
    return $output;
  }
}
