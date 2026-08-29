--TEST--
Mustache archived-template benchmark bridge exposes native arginfo
--SKIPIF--
<?php
if (!extension_loaded('mustache')) {
    die('skip mustache extension not loaded');
}
if (!method_exists(Mustache::class, 'benchmarkSerializeArchive')
    && !method_exists(Mustache::class, 'benchmarkRenderArchive')) {
    die('skip benchmark bridge disabled');
}
?>
--FILE--
<?php
foreach (['benchmarkSerializeArchive', 'benchmarkRenderArchive'] as $methodName) {
    $method = new ReflectionMethod(Mustache::class, $methodName);
    printf(
        "%s required=%d total=%d return=%s\n",
        $methodName,
        $method->getNumberOfRequiredParameters(),
        $method->getNumberOfParameters(),
        (string) $method->getReturnType(),
    );
    foreach ($method->getParameters() as $parameter) {
        printf(
            "  %s:%s %s%s\n",
            $parameter->getName(),
            (string) $parameter->getType(),
            $parameter->isOptional() ? 'optional' : 'required',
            $parameter->isDefaultValueAvailable()
                ? ' default=' . json_encode($parameter->getDefaultValue())
                : '',
        );
    }
}
?>
--EXPECT--
benchmarkSerializeArchive required=1 total=2 return=string
  tmpl:string required
  partials:array optional default=[]
benchmarkRenderArchive required=2 total=2 return=?string
  archive:string required
  vars:mixed required
