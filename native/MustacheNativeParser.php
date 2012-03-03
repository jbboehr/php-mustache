<?php

class MustacheNativeParser
{
  const NODE_ROOT = 1;
  const NODE_OUTPUT = 2;
  const NODE_TAG = 3;
  const NODE_SECTION = 4;
  
  const FLAG_ESCAPE = 1;
  const FLAG_NEGATE = 2;
  const FLAG_SECTION_START = 4;
  const FLAG_SECTION_STOP = 8;
  const FLAG_COMMENT = 16;
  const FLAG_PARTIAL = 32;
  const FLAG_INLINE_PARTIAL = 64;
  
  static public function parse($tokens)
  {
    $tree = new stdClass;
    $tree->type = MustacheNativeParser::NODE_ROOT;
    $tree->name = 'root';
    $tree->data = '';
    $tree->children = array();
    
    $list = array();
    
    $canHaveChildren = self::FLAG_SECTION_START | self::FLAG_NEGATE | self::FLAG_INLINE_PARTIAL;
    
    $depth = 0;
    $stack = array();
    $stack[$depth] = $tree;
    
    $inTag = false;
    $currentTagName = null;
    $flags = 0;
    
    foreach( array_keys($tokens) as $i ) {
      $token = &$tokens[$i];
      if( $inTag ) {
        switch( $token['type'] ) {
          case MustacheNativeTokenizer::TOKEN_START_TAG:
            throw new Exception('Whoops');
            break;
          case MustacheNativeTokenizer::TOKEN_STOP_TAG:
            // Add node
            $node = new stdClass;
            if( $flags & $canHaveChildren ) {
              $node->type = self::NODE_SECTION;
            } else {
              $node->type = self::NODE_TAG;
            }
            $node->data = $currentTagName;
            $node->flags = $flags;
            $node->children = array();
            $list[] = $node;
            
            // Cleanup
            $inTag = false;
            $currentTagName = null;
            $flags = 0;
            break;
          case MustacheNativeTokenizer::TOKEN_STRING:
            $currentTagName = $token['data'];
            break;
          
          case MustacheNativeTokenizer::TOKEN_START_SECTION:
            $flags |= self::FLAG_SECTION_START;
            break;
          case MustacheNativeTokenizer::TOKEN_STOP_SECTION:
            $flags |= self::FLAG_SECTION_STOP;
            break;
          case MustacheNativeTokenizer::TOKEN_ESCAPE:
            $flags |= self::FLAG_ESCAPE;
            break;
          case MustacheNativeTokenizer::TOKEN_NEGATION:
            $flags |= self::FLAG_NEGATE;
            break;
          case MustacheNativeTokenizer::TOKEN_COMMENT:
            $flags |= self::FLAG_COMMENT;
            break;
          case MustacheNativeTokenizer::TOKEN_PARTIAL:
            $flags |= self::FLAG_PARTIAL;
            break;
          case MustacheNativeTokenizer::TOKEN_INLINE_PARTIAL:
            $flags |= self::FLAG_INLINE_PARTIAL;
            break;
          default:
            throw new Exception('Unknown token: ' . $token['name']);
            break;
        }
      } else {
        switch( $token['type'] ) {
          case MustacheNativeTokenizer::TOKEN_START_TAG:
            $inTag = true;
            $currentTagName = null;
            $flags = 0;
            break;
          case MustacheNativeTokenizer::TOKEN_STOP_TAG:
            throw new Exception('Whoops');
            break;
          case MustacheNativeTokenizer::TOKEN_OUTPUT:
            $list[] = $node = new stdClass;
            $node->type = self::NODE_OUTPUT;
            $node->data = $token['data'];
            $node->whitespace = $token['whitespace'];
            break;
          default:
            throw new Exception('Unknown token: ' . $token['name']);
            break;
        }
      }
    }
    
    // Treeify
    foreach( $list as $node ) {
      switch( $node->type ) {
        case self::NODE_TAG:
        case self::NODE_SECTION:
          if( $node->flags & self::FLAG_SECTION_STOP ) {
            unset($stack[$depth]);
            $depth--;
          } else {
            $stack[$depth]->children[] = $node;
            if( $node->flags & $canHaveChildren ) {
              $depth++;
              $stack[$depth] = $node;
            }
          }
          break;
        case self::NODE_OUTPUT:
          $stack[$depth]->children[] = $node;
          break;
      }
    }
    
    return $tree;
  }
}