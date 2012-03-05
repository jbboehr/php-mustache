<?php

// Meh
error_reporting(E_ALL | E_STRICT);
ini_set('memory_limit', '256M');
ini_set('display_errors', true);

// Argv
if( !empty($argv[1]) && is_dir($argv[1]) ) {
  $specDir = $argv[1];
} else if( is_dir('./specs') ) {
  $specDir = './specs';
} else if( is_dir('./spec/specs') ) {
  $specDir = './spec/specs';
} else {
  echo 'Unable to find specs' . PHP_EOL;
  exit(1);
}

$specs = array();
foreach( scandir($specDir) as $file ) {
  if( $file[0] == '~' ) {
    continue;
  }
  if( strlen($file) > 5 && substr($file, -5) == '.json' ) {
    $specs[] = substr($file, 0, -5);
  }
}

$specData = array();
foreach( $specs as $spec ) {
  $specData[$spec] = json_decode(file_get_contents($specDir . DIRECTORY_SEPARATOR . $spec . '.json'), true);
}

// Generate tests
foreach( $specData as $spec => $data ) {
  $tests = $data['tests'];
  foreach( $tests as $test ) {
    $output = '';
    $output .= '--TEST--' . PHP_EOL;
    $output .= $test['name'] . PHP_EOL;
    $output .= '--DESCRIPTION--' . PHP_EOL;
    $output .= $test['desc'] . PHP_EOL;
    $output .= '--SKIPIF--' . PHP_EOL;
    $output .= "<?php if(!extension_loaded('mustache')) die('skip '); ?>" . PHP_EOL;
    $output .= '--FILE--' . PHP_EOL;
    $output .= '<?php' . PHP_EOL;
    // MAIN
    $output .= '$test = ' . var_export($test, true) . ';' . PHP_EOL;
    $output .= '$mustache = new Mustache();' . PHP_EOL;
    $output .= 'echo $mustache->render($test["template"], $test["data"]);' . PHP_EOL;
    // END MAIN
    $output .= '?>' . PHP_EOL;
//    $output .= '--EXPECT--' . PHP_EOL;
//    $output .= $test['expected'];
    $output .= '--EXPECTREGEX--' . PHP_EOL;
    $tmp = array();
    foreach( preg_split('/\s+/', $test['expected']) as $chunk ) {
      $tmp[] = preg_quote($chunk, '/');
    }
    $output .= join("\s+", $tmp);
    
    file_put_contents('./tests/mustachespec_' . $spec . '_' . $test['name'] . '.phpt', $output);
  }
}
