--TEST--
Mustache delimiter setters preserve working settings after rejected changes
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip '); ?>
--FILE--
<?php
$mustache = new Mustache();
$mustache->setStartSequence('<%');
$mustache->setStopSequence('%>');

foreach (['setStartSequence', 'setStopSequence'] as $setter) {
    foreach (['empty' => '', 'array' => []] as $label => $value) {
        echo "$setter/$label: ";
        try {
            $mustache->$setter($value);
            echo "accepted\n";
        } catch (Throwable $error) {
            echo get_class($error), "\n";
        }
        echo $mustache->getStartSequence(), '|', $mustache->getStopSequence(), "\n";
        echo $mustache->render('Hello <%name%>', ['name' => 'Ada']), "\n";
    }
}

$mustache->setStartSequence('[[');
$mustache->setStopSequence(']]');
echo $mustache->getStartSequence(), '|', $mustache->getStopSequence(), "\n";
echo $mustache->render('Hello [[name]]', ['name' => 'Ada']), "\n";
?>
--EXPECT--
setStartSequence/empty: MustacheException
<%|%>
Hello Ada
setStartSequence/array: TypeError
<%|%>
Hello Ada
setStopSequence/empty: MustacheException
<%|%>
Hello Ada
setStopSequence/array: TypeError
<%|%>
Hello Ada
[[|]]
Hello Ada
