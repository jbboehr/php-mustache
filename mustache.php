<?php

class Mustache
{
  /**
   * Constructor
   */
  public function __construct() {}
  
  /**
   * Gets whether to escape HTML by default. Defaults to true
   * 
   * @return boolean Whether to escapt HTML by default 
   */
  public function getEscapeByDefault() {}
  
  /**
   * Gets the start delimiter. Defaults to "{{"
   * 
   * @return string The start delimiter
   */
  public function getStartSequence() {}
  
  /**
   * Gets the stop delimiter. Defaults to "}}"
   * 
   * @return string The stop delimiter
   */
  public function getStopSequence() {}
  
  /**
   * Sets whether to escape HTML by default
   * 
   * @param boolean $flag 
   * @return boolean true on success, false on failure
   */
  public function setEscapeByDefault($flag) {}
  
  /**
   * Sets the start delimiter.
   * 
   * @param type $start 
   * @return boolean true on success, false on failure
   */
  public function setStartSequence($start) {}
  
  /**
   * Sets the stop delimiter.
   * 
   * @param string $stop 
   * @return boolean true on success, false on failure
   */
  public function setStopSequence($stop) {}
  
  /**
   * Tokenizes and parses a template and returns a class representing it.
   * 
   * @param mixed $tmpl The input template. May be a string or an instance 
   *                    of MustacheTemplate
   * @return MustacheAST The compiled template
   */
  public function parse($tmpl) {}
  
  /**
   * Renders a template
   * 
   * @param mixed $tmpl The input template. May be a string, or an instance of
   *                    MustacheTemplate or MustacheAST
   * @param mixed $data The input data. May be any array, scalar, or object, 
   *                    or an instance of MustacheData
   * @param mixed $partials (Optional) The template partials. Must be an array
   *                        with each value either a string, or an instance of
   *                        MustacheTemplate or MustacheAST
   * @return false|string The string output, or false on failure
   */
  public function render($tmpl, $data, $partials = null) {}
  
  /**
   * Compiles a template and returns an array representing the internal 
   * structure.
   * 
   * @param string $tmpl The input template
   * @return array The compiled template structure
   */
  public function tokenize($tmpl) {}
  
  /**
   * Converts an array into the internal data structure and back. Used for
   * debugging.
   * 
   * @param array $data The input data
   * @return array The converted data structure
   */
  public function debugDataStructure(array $data) {}
}

/**
 * Represents the parsed AST
 */
class MustacheAST
{
  /**
   * Constructor.
   * 
   * @param string $binaryString The serialized AST string
   */
  public function __construct($binaryString = null) {}
  
  /**
   * Stores the AST in a string property
   * 
   * @return array
   */
  public function __sleep() {}
  
  /**
   * Compiles a template and returns an array representing the internal 
   * structure. Alias of Mustache::tokenize()
   * 
   * @return array The compiled template structure
   */
  public function toArray() {}
  
  /**
   * Gets the serialized binary AST
   * 
   * @return string The template string 
   */
  public function __toString() {}
  
  /**
   * Restores the string properies to the internal C structure
   * 
   * @return void
   */
  public function __wakeup() {}
}

class MustacheCode
{
  /**
   * Constructor
   * 
   * @param string $codeString
   */
  public function __construct($codeString) {}
  
  /**
   * Returns a readable version of the bytecode
   * 
   * @return string
   */
  public function toReadableString() {}
  
  /**
   * Stores the bytecode in a string property for serialization
   * 
   * @return array
   */
  public function __sleep() {}
  
  /**
   * Converts the bytecode to a string
   */
  public function __toString() {}
  
  /**
   * Restores the string properies to the internal C structure
   * 
   * @return void
   */
  public function __wakeup() {}
}

class MustacheTemplate
{
  /**
   * Constructor.
   * 
   * @param string $tmpl The input template
   */
  public function __construct($tmpl = null) {}
  
  /**
   * Stores the C uncompiled and compiled templates in string properties
   * 
   * @return array
   */
  public function __sleep() {}
  
  /**
   * Sets the internal compiled template to a binary string representing the 
   * internal structure.
   * 
   * @return void
   */
  public function setFromBinary($binaryString) {}
  
  /**
   * Compiles a template and returns an array representing the internal 
   * structure. Alias of Mustache::tokenize()
   * 
   * @return array The compiled template structure
   */
  public function toArray() {}
  
  /**
   * Compiles a template and returns a binary string representing the internal 
   * structure.
   * 
   * @return string The serialized compiled template structure
   */
  public function toBinary() {}
  
  /**
   * Gets the string version of the template
   * 
   * @return string The template string 
   */
  public function __toString() {}
  
  /**
   * Restores the string properies to the internal C structure
   * 
   * @return void
   */
  public function __wakeup() {}
}

class MustacheData
{
  /**
   * Constructor
   * 
   * @param mixed $data The input data
   */
  public function __construct($data) {}
  
  /**
   * Gets the internal data
   * 
   * @return mixed The internal data 
   */
  public function toValue() {}
}

class MustacheException extends Exception {}

class MustacheParserException extends MustacheParserException
{
  /**
   * The line number of the parse error
   * 
   * @var integer
   */
  public $templateLineNo;
  
  /**
   * The character number of the parse error
   * 
   * @var integer
   */
  public $templateCharNo;
}
