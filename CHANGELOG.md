# Changelog

All Notable changes to `php-mustache` will be documented in this file.

Updates should follow the [Keep a CHANGELOG](http://keepachangelog.com/) principles.

## [Unreleased]

### Fixed
- Memory leak ([GH-57](https://github.com/jbboehr/php-mustache/issues/57))

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


[Unreleased]: https://github.com/jbboehr/php-mustache/compare/v0.9.0...HEAD
[0.9.0]: https://github.com/jbboehr/php-mustache/compare/v0.8.1...v0.9.0
[0.8.1]: https://github.com/jbboehr/php-mustache/compare/v0.8.0...v0.8.1
[0.8.0]: https://github.com/jbboehr/php-mustache/compare/v0.7.4...v0.8.0
[0.7.4]: https://github.com/jbboehr/php-mustache/compare/v0.7.3...v0.7.4
[0.7.3]: https://github.com/jbboehr/php-mustache/compare/v0.7.2...v0.7.3
[0.7.2]: https://github.com/jbboehr/php-mustache/compare/v0.7.1...v0.7.2
[0.7.1]: https://github.com/jbboehr/php-mustache/compare/v0.7.0...v0.7.1
[0.7.0]: https://github.com/jbboehr/php-mustache/compare/v0.6.1...v0.7.0

