# Examples

## Loading templates from a directory

This loader treats every `.mustache` file under a directory as a template or
partial. Nested paths become partial names such as `shared/header`.

```php
final class MustacheTemplateLoader
{
    private Mustache $mustache;
    private string $directory;

    public function __construct(string $directory, ?Mustache $mustache = null)
    {
        $resolvedDirectory = realpath($directory);
        if ($resolvedDirectory === false || !is_dir($resolvedDirectory)) {
            throw new InvalidArgumentException('Template directory does not exist: ' . $directory);
        }

        $this->directory = $resolvedDirectory;
        $this->mustache = $mustache ?? new Mustache();
    }

    public function render(string $template, mixed $data): string
    {
        $templates = $this->loadTemplates();
        if (!array_key_exists($template, $templates)) {
            throw new InvalidArgumentException('Template does not exist: ' . $template);
        }

        return $this->mustache->render($templates[$template], $data, $templates);
    }

    /** @return array<string, string> */
    private function loadTemplates(): array
    {
        $templates = [];
        $directoryPrefix = rtrim($this->directory, DIRECTORY_SEPARATOR) . DIRECTORY_SEPARATOR;
        $files = new RecursiveIteratorIterator(
            new RecursiveDirectoryIterator($this->directory, FilesystemIterator::SKIP_DOTS),
        );

        foreach ($files as $file) {
            if (!$file->isFile() || $file->getExtension() !== 'mustache') {
                continue;
            }

            $relativePath = substr($file->getPathname(), strlen($directoryPrefix));
            $template = substr($relativePath, 0, -strlen('.mustache'));
            $template = str_replace(DIRECTORY_SEPARATOR, '/', $template);
            $source = file_get_contents($file->getPathname());
            if ($source === false) {
                throw new RuntimeException('Could not read template: ' . $file->getPathname());
            }

            $templates[$template] = $source;
        }

        return $templates;
    }
}
```

Given `/usr/local/lib/templates/topview.mustache`:

```mustache
Hello {{name}}
{{> shared/footer}}
```

And `/usr/local/lib/templates/shared/footer.mustache`:

```mustache
You have just won {{value}} dollars!
```

Render `topview` with:

```php
$loader = new MustacheTemplateLoader('/usr/local/lib/templates');

echo $loader->render('topview', [
    'name' => 'John',
    'value' => 10000,
]);
```

The result is:

```text
Hello John
You have just won 10000 dollars!
```

## Persisting parsed templates

Parsed templates can be cached as binary strings and restored without parsing
the source again:

```php
$mustache = new Mustache();
$ast = $mustache->parse('Hello {{name}}');

$cache->set('greeting', $ast->toBinary());

$cached = MustacheAST::fromBinary($cache->get('greeting'));
echo $mustache->render($cached, ['name' => 'John']);
```

The binary format is provided by libmustache. Cache entries should therefore be
invalidated when libmustache is upgraded.
