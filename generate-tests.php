<?php

// Meh
error_reporting(E_ALL | E_STRICT);
ini_set('memory_limit', '256M');
ini_set('display_errors', true);
define('MY_EOL', "\n"); // PHP_EOL

if( !extension_loaded('yaml') ) {
  echo 'Requires php-yaml' . PHP_EOL;
  exit(1);
}

// Argv
if( !empty($argv[1]) && is_dir($argv[1]) ) {
  $specDir = $argv[1];
} else if( is_dir('./spec/specs') ) {
  $specDir = './spec/specs';
} else {
  echo 'Unable to find specs' . MY_EOL;
  exit(1);
}

$specs = array();
$specData = array();
foreach( scandir($specDir) as $file ) {
  //if( strlen($file) > 5 && substr($file, -5) ==  '.json' ) {
  if( strlen($file) > 4 && substr($file, -4) == '.yml' ) {
    $spec = substr($file, 0, -4);
    $specs[] = $spec;
    
    $contents = file_get_contents($specDir . DIRECTORY_SEPARATOR . $file);
    //$specData[$spec] = json_decode($contents, true);
    $specData[$spec] = yaml_parse($contents);
  }
}

if( empty($specs) ) {
  echo 'No specs found in specified directory' . MY_EOL;
  exit(1);
}

// Generate tests
foreach( $specData as $spec => $data ) {
  $tests = $data['tests'];
  foreach( $tests as $test ) {
    $lambda = null;
    if (isset($test['data']['lambda'])) {
      if (!isset($test['data']['lambda']['php'])) {
        continue;
      }

      $lambda = $test['data']['lambda']['php'];
      $test['data']['lambda'] = true;
    }

    $output = '';
    $output .= '--TEST--' . MY_EOL;
    $output .= $test['name'] . MY_EOL;
    $output .= '--DESCRIPTION--' . MY_EOL;
    $output .= $test['desc'] . MY_EOL;
    $output .= '--SKIPIF--' . MY_EOL;
    $output .= "<?php if(!extension_loaded('mustache')) die('skip '); ?>" . MY_EOL;
    $output .= '--FILE--' . MY_EOL;
    $output .= '<?php' . MY_EOL;
    // MAIN
    $output .= '$test = ' . str_replace('\'lambda\' => true,', '\'lambda\' => function ($text = \'\') { ' . $lambda . ' },', var_export($test, true)) . ';' . MY_EOL;
    $output .= '$mustache = new Mustache();' . MY_EOL;
    if( !empty($test['partials']) && is_array($test['partials']) ) {
      $output .= 'echo "<render>", $mustache->render($test["template"], $test["data"], $test["partials"]), "</render>";' . MY_EOL;
    } else {
      $output .= 'echo "<render>", $mustache->render($test["template"], $test["data"]), "</render>";' . MY_EOL;
    }
    // END MAIN
    $output .= '?>' . MY_EOL;
    $output .= '--EXPECT--' . MY_EOL;
    $output .= '<render>' . $test['expected'] . '</render>';
    
    $cleanName = strtolower(trim(preg_replace('/[^a-zA-Z0-9]+/', '-', $test['name']), '-'));
    file_put_contents('./tests/mustache-spec-' . $spec . '-' . $cleanName . '.phpt', $output);
  }
}
