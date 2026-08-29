<?php

/**
 * Renders and compiles Mustache templates.
 */
class Mustache
{
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
     * Passing an existing MustacheTemplate or MustacheAST validates it and
     * returns true.
     *
     * @throws TypeError If the template has an unsupported PHP type.
     * @throws ValueError If the template object is not initialized.
     * @throws MustacheParserException If the template source is invalid.
     */
    public function parse(string|MustacheTemplate|MustacheAST $tmpl): MustacheAST|bool
    {
    }

    /**
     * Renders a template with the supplied data and partials.
     *
     * @param array<string, string|MustacheTemplate|MustacheAST>|null $partials
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
     * Tokenizes a source template into its internal tree representation.
     *
     * @return array<string, mixed>
     */
    public function tokenize(string $tmpl): array
    {
    }

    /**
     * Converts data to libmustache's internal representation and back.
     */
    public function debugDataStructure(mixed $vars): mixed
    {
    }
}

/**
 * Owns a parsed Mustache template tree.
 */
class MustacheAST
{
    /** @var string|null Serialized AST storage used by PHP serialization. */
    protected $binaryString = null;

    /**
     * Constructs an AST from libmustache's binary representation.
     */
    public function __construct(?string $vars = null)
    {
    }

    /**
     * Stores the binary AST for PHP serialization.
     *
     * @return list<string>
     */
    public function __sleep(): array
    {
    }

    /**
     * Returns the parsed template tree.
     *
     * @return array<string, mixed>
     */
    public function toArray(): array
    {
    }

    /**
     * Returns libmustache's binary AST representation.
     */
    public function __toString(): string
    {
    }

    /**
     * Restores the native AST after PHP unserialization.
     */
    public function __wakeup(): void
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
class MustacheData
{
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
class MustacheLambdaHelper
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
