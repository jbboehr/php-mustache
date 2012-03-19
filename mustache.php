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
   * Compiles a template and returns a class representing it.
   * 
   * @param string|MustacheTemplate $tmpl The input template
   * @return MustacheTemplate|bool The compiled template if given a string, or 
   *                               true if given an instance of MustacheTemplate
   */
  public function compile($tmpl) {}
  
  /**
   * Compiles a template and returns an array representing the internal 
   * structure.
   * 
   * @param string $tmpl The input template
   * @return array The compiled template structure
   */
  public function tokenize($tmpl) {}
  
  /**
   * Renders a template
   * 
   * @param string|MustacheTemplate $tmpl The input template
   * @param array|MustacheData $data The input data
   * @param array $partials (Optional) The template partials
   * @return false|string The string output, or false on failure
   */
  public function render($tmpl, $data, $partials = null) {}
  
  /**
   * Converts an array into the internal data structure and back. Used for
   * debugging.
   * 
   * @param array $data The input data
   * @return array The converted data structure
   */
  public function debugDataStructure(array $data) {}
}

class MustacheTemplate
{
  /**
   * Constructor.
   * 
   * @param string $tmpl The input template
   */
  public function __construct($tmpl) {}
  
  /**
   * Compiles a template and returns an array representing the internal 
   * structure. Alias of Mustache::tokenize()
   * 
   * @return array The compiled template structure
   */
  public function toArray() {}
  
  /**
   * Gets the string version of the template
   * 
   * @return string The template string 
   */
  public function __toString() {}
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
