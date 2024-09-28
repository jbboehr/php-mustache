
# Examples

You can adapt this Mustache_Template_Loader class or use it as an
example of how to interface with whatever framework you use. The MustacheAST
type for example can serialized/unserialized to APCU or other cache.

```php
class Mustache_Template_Loader {
  public $partials = [];
  public $partial_pathname_resolver = false;
  public $mustache_instance = false;

  /**
   * Loads and renders a template from a path.
   *
   * @param string   $tmplpath The input template filesystem path.
   * @param mixed    $data     The data argument to Mustache::render.
   * @param callable $callback Resolves a partial name to its filesystem path.
   *
   * @return false|string The string output, or false on failure
   */
  public static function load_and_render($tmplpath, $data, $callback) {
    $loader = new Mustache_Template_Loader();
    $loader->mustache_instance = new \Mustache();
    $loader->partial_pathname_resolver = $callback;
    $template_string = file_get_contents($tmplpath);
    $template_ast = $loader->mustache_instance->parse($template_string);
    $template_ast_array = $template_ast->toArray();
    $loader->resolve_partials($template_ast_array);
    return $loader->mustache_instance->render($template_ast, $data, $loader->partials);
  }

  public function resolve_partials($ast_array) {
    if (($ast_array['type'] ?? null) === 512) {
      // The libmustache src/node.hpp has enum Type TypePartial = 512
      $partial_name = $ast_array['data'];
      $partial_ast = $this->partials[$partial_name] ?? null;
      if ($partial_ast !== null) {
        // this check prevents the performing of extra work
        // and by corollary protects against indefinite recursion.
        return;
      }
      $this->partials[$partial_name] = "";
      $partial_pathname = ($this->partial_pathname_resolver)($partial_name);
      $partial_string = file_get_contents($partial_pathname);
      $partial_ast = $this->mustache_instance->parse($partial_string);
      $this->partials[$partial_name] = $partial_ast;
      $this->resolve_partials($partial_ast->toArray());
    } else {
      // For simplicity the AST node types are ignored here.
      foreach ($ast_array as $ast_array_value) {
        if (is_array($ast_array_value)) {
          $this->resolve_partials($ast_array_value);
        }
      }
    }
  }
}
```

An example use of this class:

```php
function template_pathname($partial_name) {
  return '/usr/local/lib/templates/' . $partial_name . '.mustache';
}

echo Mustache_Template_Loader::load_and_render(
       template_pathname('topview'),
       $data,
       'template_pathname'
     );
```
