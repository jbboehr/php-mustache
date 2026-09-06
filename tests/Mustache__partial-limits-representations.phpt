--TEST--
Mustache partial text limits account for source, wrappers, source ASTs, and binary AST strings
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
if (!method_exists(Mustache::class, 'setPartialLimits')) die("missing setPartialLimits\n");
$m = new Mustache();
$source = "{{! keep }}\0é";
$ast = $m->parse($source);
// Each source representation costs 15 bytes including its one-byte map name.
foreach (['string' => $source, 'wrapper' => new MustacheTemplate($source), 'AST' => $ast] as $kind => $partial) {
    foreach ([14, 15] as $limit) {
        $m->setPartialLimits(maxTextBytes: $limit);
        try {
            $output = $m->render('{{>a}}', [], ['a' => $partial]);
            echo "$kind/$limit: ", bin2hex($output), "\n";
        } catch (ValueError $e) {
            echo "$kind/$limit: rejected\n";
        }
    }
}

$m->setPartialLimits();
$binary = MustacheAST::fromBinary($m->parse('{{a.b}}')->toBinary());
// One map-name byte + node data "a.b" (3) + data parts "a", "b" (2).
foreach ([5, 6] as $limit) {
    $m->setPartialLimits(maxTextBytes: $limit);
    try {
        echo "binary/$limit: ", $m->render('{{>p}}', ['a' => ['b' => 'ok']], ['p' => $binary]), "\n";
    } catch (ValueError $e) {
        echo "rejected\n";
    }
}

$m->setPartialLimits();
$textAst = $m->parse('X');
$binaryText = MustacheAST::fromBinary($textAst->toBinary());
foreach (['source AST' => $textAst, 'binary AST' => $binaryText] as $kind => $partial) {
    foreach ([3, 4] as $limit) {
        $m->setPartialLimits(maxTextBytes: $limit);
        try {
            $output = $m->render('{{>a}}/{{>b}}', [], ['a' => $partial, 'b' => $partial]);
            echo "$kind/$limit: $output\n";
        } catch (ValueError $e) {
            echo "$kind/$limit: rejected\n";
        }
    }
}
$m->setPartialLimits(3, 6);
echo 'mixed: ', $m->render('{{>a}}{{>b}}{{>c}}', [], [
    'a' => new MustacheTemplate('A'), 'b' => $textAst, 'c' => $binaryText,
]), "\n";
?>
--EXPECT--
string/14: rejected
string/15: 00c3a9
wrapper/14: rejected
wrapper/15: 00c3a9
AST/14: rejected
AST/15: 00c3a9
binary/5: rejected
binary/6: ok
source AST/3: rejected
source AST/4: X/X
binary AST/3: rejected
binary AST/4: X/X
mixed: AXX
