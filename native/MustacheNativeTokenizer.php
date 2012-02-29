<?php

class MustacheNativeTokenizer
{
  const TOKEN_OUTPUT = 0;
  const TOKEN_STRING = 1;
  const TOKEN_START_TAG = 2;
  const TOKEN_STOP_TAG = 3;
  const TOKEN_START_SECTION = 4;
  const TOKEN_STOP_SECTION = 5;
  const TOKEN_ESCAPE = 6;
  const TOKEN_NEGATION = 7;
  const TOKEN_COMMENT = 8;
  
  const TOKEN_ROOT = 100;
  
  static public function tokenize($tmpl, $start = '{{', $stop = '}}')
  {
    $tmplL = strlen($tmpl);
    $startC = $start[0];
    $startL = strlen($start);
    $stopC = $stop[0];
    $stopL = strlen($stop);
    
    $pos = 0;
    $char = null;
    $inTag = false;
    $inComment = false;
    $startLine = null;
    $startChar = null;
    $skipUntil = null;
    
    $lineNo = 0;
    $charNo = 0;
    
    $buffer = '';
    $tokens = array();
    
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
      
      if( $inTag ) { // INSIDE TAG
        switch( $char ) {
          case $stopC: // STOP TAG
            if( substr($tmpl, $pos, $stopL) == $stop ) {
              // Close previous buffer
              if( '' !== $buffer ) {
                if( strlen($buffer) != strcspn($buffer, '{}#&!^/') ) {
                  self::_errorWithLocation('Invalid char in tag', $startLine, $startChar);
                }
                $data = trim($buffer); // TRIM THE VARIABLE BUFFER;
                if( !$data ) {
                  self::_errorWithLocation('Empty tag', $startLine, $startChar);
                }
                $tokens[] = array(
                  'type' => self::TOKEN_STRING,
                  'name' => 'string',
                  'data' => $data,
                  'startLineNo' => $startChar,
                  'startCharNo' => $startChar,
                  'lineNo' => $lineNo,
                  'charNo' => $charNo,
                );
                $buffer = '';
              }
              // Add token
              $tokens[] = array(
                'type' => self::TOKEN_STOP_TAG,
                'name' => 'stop_tag',
                'data' => $stop,
                'lineNo' => $lineNo,
                'charNo' => $charNo,
              );
              // Now outside tag
              $inTag = false;
              $inComment = false;
              $startLine = $lineNo;
              $startChar = $charNo;
              $skipUntil = $pos + $stopL - 1;
            }
            break;
          case '#': // START SECTION
            if( !$inComment ) {
              if( strlen($buffer) !== 0 ) {
                self::_errorWithLocation('Unexpected token: ' . $char, $lineNo, $charNo);
              } else {
                $tokens[] = array(
                  'type' => self::TOKEN_START_SECTION,
                  'name' => 'start_section',
                  'data' => $char,
                  'lineNo' => $lineNo,
                  'charNo' => $charNo,
                );
                $skipUntil = $pos;
              }
            }
            break;
          case '/': // STOP SECTION
            if( !$inComment ) {
              if( strlen($buffer) !== 0 ) {
                self::_errorWithLocation('Unexpected token: ' . $char, $lineNo, $charNo);
              } else {
                $tokens[] = array(
                  'type' => self::TOKEN_STOP_SECTION,
                  'name' => 'stop_section',
                  'data' => $char,
                  'lineNo' => $lineNo,
                  'charNo' => $charNo,
                );
                $skipUntil = $pos;
              }
            }
            break;
          case '&': // ESCAPE/UNESCAPE
            if( !$inComment ) {
              if( strlen($buffer) !== 0 ) {
                self::_errorWithLocation('Unexpected token: ' . $char, $lineNo, $charNo);
              } else {
                $tokens[] = array(
                  'type' => self::TOKEN_ESCAPE,
                  'name' => 'escape',
                  'data' => $char,
                  'lineNo' => $lineNo,
                  'charNo' => $charNo,
                );
                $skipUntil = $pos;
              }
            }
            break;
          case '^': // SECTION NEGATION
            if( !$inComment ) {
              if( strlen($buffer) !== 0 ) {
                self::_errorWithLocation('Unexpected token: ' . $char, $lineNo, $charNo);
              } else {
                $tokens[] = array(
                  'type' => self::TOKEN_NEGATION,
                  'name' => 'negation',
                  'data' => $char,
                  'lineNo' => $lineNo,
                  'charNo' => $charNo,
                );
                $skipUntil = $pos;
              }
            }
            break;
          case '!': // COMMENT
            if( !$inComment ) {
              if( strlen($buffer) !== 0 ) {
                self::_errorWithLocation('Unexpected token: ' . $char, $lineNo, $charNo);
              } else {
                $tokens[] = array(
                  'type' => self::TOKEN_COMMENT,
                  'name' => 'comment',
                  'data' => $char,
                  'lineNo' => $lineNo,
                  'charNo' => $charNo,
                );
                $skipUntil = $pos;
                $inComment = true;
              }
            }
            break;
        }
      } else { // OUTSIDE TAG
        switch( $char ) {
          case $startC:
            if( substr($tmpl, $pos, $startL) == $start ) {
              // Close previous buffer
              if( '' !== $buffer ) {
                $tokens[] = array(
                  'type' => self::TOKEN_OUTPUT,
                  'name' => 'output',
                  'data' => $buffer,
                  'startLineNo' => $startLine,
                  'startCharNo' => $startChar,
                  'lineNo' => $lineNo,
                  'charNo' => $charNo,
                );
                $buffer = '';
              }
              // Add token
              $tokens[] = array(
                'type' => self::TOKEN_START_TAG,
                'name' => 'start_tag',
                'data' => $start,
                'lineNo' => $lineNo,
                'charNo' => $charNo,
              );
              // Now inside tag
              $inTag = true;
              $startLine = $lineNo;
              $startChar = $charNo;
              $skipUntil = $pos + $startL - 1;
            }
            break;
        }
      }
      
      // Append to buffer
      if( null === $skipUntil ) {
        $buffer .= $char;
      }
    }
    
    return $tokens;
  }
  
  protected static function _errorWithLocation($msg, $line, $char)
  {
    $msg .= ' [Line: ' . $line . ', Char: ' . $char . ']';
    throw new Exception($msg);
  }
}
