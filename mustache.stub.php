<?php

/**
 * Renders and compiles Mustache templates.
 */
class Mustache
{
#if PHP_VERSION_ID < 80100
    /** @internal PHP 8.0 serialization-denial hook. */
    final public function __serialize(): array
    {
    }

    /** @internal PHP 8.0 serialization-denial hook. */
    final public function __unserialize(array $data): void
    {
    }
#endif

    public function __construct()
    {
    }

    /**
     * Returns whether interpolation is escaped by default.
     */
    public function getEscapeByDefault(): bool
    {
    }

    /**
     * Returns the opening delimiter. The default is "{{".
     */
    public function getStartSequence(): string
    {
    }

    /**
     * Returns the closing delimiter. The default is "}}".
     */
    public function getStopSequence(): string
    {
    }

    /**
     * Sets whether interpolation is escaped by default.
     */
    public function setEscapeByDefault(bool|int $escapeByDefault): bool
    {
    }

    /**
     * Sets the opening delimiter.
     */
    public function setStartSequence(string $startSequence): bool
    {
    }

    /**
     * Sets the closing delimiter.
     */
    public function setStopSequence(string $stopSequence): bool
    {
    }

    /**
     * Parses a template into an AST.
     *
     * Passing an existing MustacheAST returns the same object. A
     * MustacheTemplate is compiled into a new AST.
     *
     * @throws TypeError If the template has an unsupported PHP type.
     * @throws ValueError If the template object is not initialized.
     * @throws MustacheParserException If the template source is invalid.
     */
    public function parse(string|MustacheTemplate|MustacheAST $tmpl): MustacheAST
    {
    }

    /**
     * Renders a template with the supplied data and partials.
     *
     * @phpstan-param array<string, string|MustacheTemplate|MustacheAST>|null $partials
     *
     * @throws TypeError If the template or data has an unsupported PHP type.
     * @throws ValueError If the template, data, or partial map is invalid.
     * @throws MustacheParserException If template or partial source is invalid.
     * @throws MustacheException If rendering fails.
     */
    public function render(
        string|MustacheTemplate|MustacheAST $str,
        mixed $vars,
        ?array $partials = null,
    ): string {
    }

    /**
     * Returns libmustache's internal parse tree for diagnostics.
     *
     * The array shape is not part of the compatibility contract and may change
     * between releases.
     *
     * @internal
     * @return array<string, mixed>
     */
    public function tokenize(string $tmpl): array
    {
    }

    /**
     * Converts data to libmustache's internal representation and back for diagnostics.
     *
     * The result is not part of the compatibility contract and is lossy for lambdas.
     *
     * @internal
     */
    public function debugDataStructure(mixed $vars): mixed
    {
    }

#if defined(PHP_MUSTACHE_ARCHIVE_BENCHMARK)
    /**
     * Serializes a template for archived-template benchmarking.
     *
     * @internal
     * @phpstan-param array<string, string|MustacheTemplate|MustacheAST> $partials
     */
    public function benchmarkSerializeArchive(string $tmpl, array $partials = []): string
    {
    }

    /**
     * Renders an archived template for benchmarking.
     *
     * @internal
     */
    public function benchmarkRenderArchive(string $archive, mixed $vars): ?string
    {
    }
#endif
}

/**
 * Owns a parsed Mustache template tree.
 */
class MustacheAST
{
    /**
     * Constructs an AST from libmustache's binary representation.
     */
    public function __construct(?string $vars = null)
    {
    }

    /**
     * Creates an AST from libmustache's binary representation.
     */
    public static function fromBinary(string $binary): static
    {
    }

    /**
     * Returns the data used by PHP serialization.
     *
     * @return array{binary: string}
     */
    public function __serialize(): array
    {
    }

    /**
     * Restores the native AST from PHP serialization data.
     *
     * Legacy payloads produced by __sleep() are also accepted.
     */
    public function __unserialize(array $data): void
    {
    }

    /**
     * Returns libmustache's internal parse tree for diagnostics.
     *
     * The array shape is not part of the compatibility contract and may change
     * between releases. Use toBinary() for persistent parsed-template caches.
     *
     * @internal
     * @return array<string, mixed>
     */
    public function toArray(): array
    {
    }

    /**
     * Returns libmustache's binary AST representation.
     */
    public function toBinary(): string
    {
    }

    /**
     * Returns libmustache's binary AST representation.
     *
     * @deprecated Use toBinary() instead.
     */
    public function __toString(): string
    {
    }

}

/**
 * Wraps a source template.
 */
class MustacheTemplate
{
    /** @var string|null */
    protected $template = null;

    public function __construct(?string $vars = null)
    {
    }

    public function __toString(): string
    {
    }
}

/**
 * Owns data converted to libmustache's native representation.
 */
final class MustacheData
{
#if PHP_VERSION_ID < 80100
    /** @internal PHP 8.0 serialization-denial hook. */
    final public function __serialize(): array
    {
    }

    /** @internal PHP 8.0 serialization-denial hook. */
    final public function __unserialize(array $data): void
    {
    }
#endif

    public function __construct(mixed $tmpl)
    {
    }

    /**
     * Converts the native data back to a PHP value.
     */
    public function toValue(): mixed
    {
    }
}

/**
 * Renders source text from inside a section lambda.
 *
 * Instances are created by the extension and are valid only for the duration
 * of the lambda callback.
 */
final class MustacheLambdaHelper
{
    private function __construct()
    {
    }

    public function render(string $tmpl): string
    {
    }
}

class MustacheException extends Exception
{
}

class MustacheParserException extends MustacheException
{
    /** @var int One-based template line, or -1 when unavailable. */
    public $templateLineNo = -1;

    /** @var int One-based template character, or -1 when unavailable. */
    public $templateCharNo = -1;
}
