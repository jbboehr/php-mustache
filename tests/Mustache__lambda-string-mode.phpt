--TEST--
Mustache lambda string mode validates configuration and initializes every instance
--SKIPIF--
<?php if (!extension_loaded('mustache')) die('skip mustache extension required'); ?>
--FILE--
<?php
declare(strict_types=1);
if (!method_exists(Mustache::class, 'setLambdaStringMode')) die("missing lambda string mode\n");
$m = new Mustache();
var_dump(Mustache::LAMBDA_STRING_TEMPLATE, Mustache::LAMBDA_STRING_LITERAL);
var_dump($m->getLambdaStringMode());
var_dump($m->setLambdaStringMode(mode: Mustache::LAMBDA_STRING_LITERAL));
foreach ([-1, 2, PHP_INT_MAX, [], '1', null] as $invalid) {
    try {
        $m->setLambdaStringMode($invalid);
        echo "accepted invalid mode\n";
    } catch (ValueError|TypeError $error) {
        echo get_class($error), ':', $m->getLambdaStringMode(), "\n";
    }
}
$m->__construct();
var_dump($m->getLambdaStringMode());
class ModeSubclass extends Mustache { public function __construct() {} }
foreach ([new Mustache(), new ModeSubclass(), (new ReflectionClass(ModeSubclass::class))->newInstanceWithoutConstructor()] as $instance) {
    echo 'default:', $instance->getLambdaStringMode(), ':', $instance->render('{{v}}', [
        'v' => fn () => '{{name}}', 'name' => 'Ada',
    ]), "\n";
}
foreach (['getLambdaStringMode', 'setLambdaStringMode'] as $name) {
    $method = new ReflectionMethod(Mustache::class, $name);
    echo $name, ':', $method->getReturnType(), ':', $method->getNumberOfRequiredParameters(),
        ':', $method->isPublic() ? 'public' : 'private', ':', $method->isStatic() ? 'static' : 'instance', "\n";
    foreach ($method->getParameters() as $parameter) {
        echo $parameter->getName(), ':', $parameter->getType(), "\n";
    }
}
$m->setLambdaStringMode(Mustache::LAMBDA_STRING_TEMPLATE);
var_dump($m->getLambdaStringMode());
?>
--EXPECT--
int(0)
int(1)
int(0)
NULL
ValueError:1
ValueError:1
ValueError:1
TypeError:1
TypeError:1
TypeError:1
int(1)
default:0:Ada
default:0:Ada
default:0:Ada
getLambdaStringMode:int:0:public:instance
setLambdaStringMode:void:1:public:instance
mode:int
int(0)
