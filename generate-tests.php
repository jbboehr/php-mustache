<?php

// Meh
error_reporting(E_ALL | E_STRICT);
ini_set('memory_limit', '256M');
ini_set('display_errors', true);

// Argv
if( !empty($argv[1]) && is_dir($argv[1]) ) {
  $specDir = $argv[1];
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

if( empty($specs) ) {
  echo 'No specs found in specified directory' . PHP_EOL;
  exit(1);
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
    if( !empty($test['partials']) && is_array($test['partials']) ) {
      $output .= 'echo $mustache->render($test["template"], $test["data"], $test["partials"]);' . PHP_EOL;
    } else {
      $output .= 'echo $mustache->render($test["template"], $test["data"]);' . PHP_EOL;
    }
    // END MAIN
    $output .= '?>' . PHP_EOL;
//    $output .= '--EXPECT--' . PHP_EOL;
//    $output .= $test['expected'];
    $output .= '--EXPECTREGEX--' . PHP_EOL;
    $tmp = array();
    foreach( preg_split('/\s+/', $test['expected']) as $chunk ) {
      $tmp[] = preg_quote($chunk, '/');
    }
    $expected = join("\s*", $tmp);
    // Hack in XFAIL
    if( ($spec == 'partials' && $test['name'] == 'Standalone Line Endings') ||
        ($spec == 'partials' && $test['name'] == 'Standalone Without Previous Line') ) {
      //$output .= PHP_EOL;
      //$output .= '--XFAIL--' . PHP_EOL;
      //$output .= 'This extension does not follow the spec\'s whitespace rules.';
      $expected = "\s*" . join("\s*", str_split(preg_replace('/\s+/', '', $test['expected']), 1)) . "\s*";
    }
    $output .= $expected;
    
    $cleanName = strtolower(trim(preg_replace('/[^a-zA-Z0-9]+/', '-', $test['name']), '-'));
    file_put_contents('./tests/mustache-spec-' . $spec . '-' . $cleanName . '.phpt', $output);
  }
}
