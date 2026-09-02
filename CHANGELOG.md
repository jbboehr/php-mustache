# Changelog

All Notable changes to `php-mustache` will be documented in this file.

Updates should follow the [Keep a CHANGELOG](http://keepachangelog.com/) principles.

## [Unreleased]

### Added

- Add PIE package metadata while retaining the existing PECL package metadata.
- Add explicit `MustacheAST::toBinary()` and `MustacheAST::fromBinary()` APIs for persistent parsed-template caches.

### Changed

- Compare generated Mustache specification output with exact whitespace boundaries instead of ignoring whitespace differences.
- Follow Mustache standalone-tag whitespace and partial-indentation rules provided by libmustache.
- Keep Valgrind out of the default flake checks and expose it as an explicit PHP 8.3 build.
- Preserve PHP null, boolean, integer, and floating-point data as typed libmustache values. Their rendered spelling now follows libmustache's typed-value rules.
- Compile source templates and source partials through libmustache's owning template handles while preserving configured delimiters.
- Store `MustacheAST` values with explicit ownership and bounded serialization.
- Clone AST-backed partials into each compatibility render so ownership never aliases PHP objects.
- Make `Mustache::render()` and invalid-input paths in `Mustache::parse()` throw `TypeError` or `ValueError` instead of emitting warnings or returning `false`, `null`, or partial output. Successful renders now always return strings.
- Make `Mustache::parse()` consistently return a `MustacheAST`, preserving the identity of existing AST values and compiling `MustacheTemplate` values.
- Expose the documented method signatures through native reflection and ship the canonical PHP stub with source packages.
- Deprecate `MustacheAST::__toString()` in the tooling stub in favor of the explicit binary export API.
- Mark `Mustache::tokenize()`, `Mustache::debugDataStructure()`, and `MustacheAST::toArray()` as unstable diagnostic APIs.
- Replace `MustacheAST::__sleep()` and `MustacheAST::__wakeup()` with `__serialize()` and `__unserialize()` while continuing to accept existing serialized payloads.

### Fixed

- Accept explicit `null` for the optional partial map, matching the public signature and an omitted argument.
- Accept `MustacheTemplate` and `MustacheAST` subclasses wherever their base classes are supported.
- Reject `MustacheTemplate` values whose stored source is not a string instead of coercing them with a warning.
- Reject recursive, over-deep, and over-budget data conversion without publishing a partial `MustacheData` value.
- Retain PHP objects and closures for as long as their converted lambda values are alive.
- Reject `MustacheData` reinitialization so callbacks cannot replace data borrowed by an active render.
- Reject malformed, over-budget, and trailing `MustacheAST` serialization data.
- Reject attempts to reinitialize a `MustacheAST` while its node may be borrowed by a render.
- Invalidate retained `MustacheLambdaHelper` values safely when their callback returns.
- Release lambda arguments and return values reliably when callbacks or string conversion fail.
- Reject serialization of `Mustache` and `MustacheData` instead of restoring objects with reset or missing native state.
- Declare parser-error location properties with `-1` defaults for unknown locations so invalid templates do not emit PHP 8.2 deprecation notices.
- Preserve literal format specifiers in exception messages and stop rendering as soon as a PHP lambda callback raises an exception.
- Fix the `mustache.default_*` INI directives, which were registered under pointer-sized truncated names and read past their default literals. Existing `php.ini` and `ini_set()` values now take effect.

## [0.9.3] - 2022-11-26

### Fixed

- PHP 8.2 compat (@remicollet)

## [0.9.2] - 2020-11-12

### Fixed

- Fixes for latest PHP 8.0

### Changed

- Drop max PHP version constraint in package.xml

## [0.9.1] - 2019-12-11

### Fixed

- Memory leak ([GH-57](https://github.com/jbboehr/php-mustache/issues/57))
- PHP 7.4 build issues ([GH-61](https://github.com/jbboehr/php-mustache/pull/61)) ([@sunpoet](https://github.com/sunpoet))

## [0.9.0] - 2019-05-27

### Added

- Preliminary PHP 7.4 and PHP 8 support

### Removed

- PHP 5 support, require at least PHP 7.1

## [0.8.1] - 2019-05-27

### Fixed

- Missing file in PECL distribution (@remicollet)

## [0.8.0] - 2018-11-11

### Added

- PHP 7.3 support (@remicollet)

### Changed

- Require libmustache >= 0.5.0 (fixes a double-free)

### Removed

- Old deprecated experimental compiler
- Support for less than PHP  5.6

## [0.7.4] - 2017-07-18

### Added

- PHP 7.2 support (@remicollet)

### Fixed

- RHEL/CentOS build issues (@remicollet)

## [0.7.3] - 2017-07-18

### Added

- PHP 7.1 support (@adambaratz and @remicollet)

### Fixed

- RHEL/CentOS build issues (@amcnea)

## [0.7.2] - 2016-04-24

### Fixed

- Indirect zval issues (@adambaratz)

## [0.7.1] - 2016-03-17

### Fixed

- Various lambda issues (@adambaratz)

## [0.7.0] - 2016-03-17

### Added

- Lambda support (@adambaratz)

[Unreleased]: https://github.com/jbboehr/php-mustache/compare/v0.9.3...HEAD
[0.9.3]: https://github.com/jbboehr/php-mustache/compare/v0.9.2...v0.9.3
[0.9.2]: https://github.com/jbboehr/php-mustache/compare/v0.9.1...v0.9.2
[0.9.1]: https://github.com/jbboehr/php-mustache/compare/v0.9.0...v0.9.1
[0.9.0]: https://github.com/jbboehr/php-mustache/compare/v0.8.1...v0.9.0
[0.8.1]: https://github.com/jbboehr/php-mustache/compare/v0.8.0...v0.8.1
[0.8.0]: https://github.com/jbboehr/php-mustache/compare/v0.7.4...v0.8.0
[0.7.4]: https://github.com/jbboehr/php-mustache/compare/v0.7.3...v0.7.4
[0.7.3]: https://github.com/jbboehr/php-mustache/compare/v0.7.2...v0.7.3
[0.7.2]: https://github.com/jbboehr/php-mustache/compare/v0.7.1...v0.7.2
[0.7.1]: https://github.com/jbboehr/php-mustache/compare/v0.7.0...v0.7.1
[0.7.0]: https://github.com/jbboehr/php-mustache/compare/v0.6.1...v0.7.0
