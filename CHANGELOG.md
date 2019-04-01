# Changelog
All notable changes to this project will be documented in this file.

## [Unreleased]
### Changed
- Fix bug with incorrect display of current value due to divider error
- Optimization of the algorithm for receiving data. Fix bug with clearing the buffer the received data timeout
- Fix bug with the lack of responses from the device when connecting.

## [1.0.1] - 2019-03-xx
### Changed
- Change some syntax for limits into xml config.
- Add SignedCommand subclass for Command. It uses for display signed values.
### Fixed
- Each time you connect to device, if it returns a "common ID", user is prompted to select a custom device model.
- Limit settings window is working correctly now. Fix bug with sending new limits without code: user could send a new limit like "P <value>".
