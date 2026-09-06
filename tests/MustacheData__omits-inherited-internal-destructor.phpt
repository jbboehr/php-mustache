--TEST--
MustacheData omits inherited internal destructor methods
--SKIPIF--
<?php
if (!extension_loaded('mustache')) die('skip ');
if (!class_exists('PharData')) die('skip PharData is unavailable');
?>
--FILE--
<?php
class ChildPharData extends PharData
{
    public string $marker = 'property';

    public function ordinaryMethod()
    {
        return 'method';
    }
}

$path = sys_get_temp_dir() . '/php-mustache-' . getmypid() . '-'
    . bin2hex(random_bytes(8)) . '.tar';

try {
    $archive = new ChildPharData($path);
    $archive->addFromString('fixture.txt', 'fixture');

    set_error_handler(static function () {
        return true;
    });
    try {
        $data = new MustacheData($archive);
        $value = $data->toValue();
    } finally {
        restore_error_handler();
    }

    echo 'property=', $value['marker'], "\n";
    echo 'ordinary=', array_key_exists('ordinaryMethod', $value)
        && $value['ordinaryMethod'] === null ? 'lambda' : 'missing', "\n";
    echo 'destructor=', array_key_exists('__destruct', $value)
        ? 'present' : 'omitted', "\n";
} finally {
    unset($value, $data, $archive);
    if (is_file($path)) {
        unlink($path);
    }
}
?>
--EXPECT--
property=property
ordinary=lambda
destructor=omitted
