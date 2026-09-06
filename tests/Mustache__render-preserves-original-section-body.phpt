--TEST--
Mustache::render() preserves exact section callback text across roots and partials
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
function reportSectionBody($label, Mustache $mustache, $template, $partial = null) {
  $captured = null;
  $data = array(
    'capture' => function ($text) use (&$captured) {
      $captured = $text;
      return '';
    },
  );
  $partials = $partial === null ? null : array('piece' => $partial);
  $mustache->render($template, $data, $partials);
  $visible = str_replace(array("\r", "\n"), array('<CR>', '<LF>'), $captured);
  echo $label, ':', $visible, "\n";
}

$mustache = new Mustache();
$body = "A {{ name }} {{! keep }} {{{name}}}\r\nB";
$section = '{{#capture}}' . $body . '{{/capture}}';
$sourceRoot = '{{>piece}}';
$astRoot = $mustache->parse($sourceRoot);
$astSection = $mustache->parse($section);

reportSectionBody('source-root', $mustache, $section);
reportSectionBody('ast-root', $mustache, $astSection);
reportSectionBody('source-root-source-partial', $mustache, $sourceRoot, $section);
reportSectionBody('ast-root-source-partial', $mustache, $astRoot, $section);
reportSectionBody('source-root-ast-partial', $mustache, $sourceRoot, $astSection);
reportSectionBody('ast-root-ast-partial', $mustache, $astRoot, $astSection);
?>
--EXPECT--
source-root:A {{ name }} {{! keep }} {{{name}}}<CR><LF>B
ast-root:A {{ name }} {{! keep }} {{{name}}}<CR><LF>B
source-root-source-partial:A {{ name }} {{! keep }} {{{name}}}<CR><LF>B
ast-root-source-partial:A {{ name }} {{! keep }} {{{name}}}<CR><LF>B
source-root-ast-partial:A {{ name }} {{! keep }} {{{name}}}<CR><LF>B
ast-root-ast-partial:A {{ name }} {{! keep }} {{{name}}}<CR><LF>B
