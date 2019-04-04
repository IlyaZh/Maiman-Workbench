# Changelog
All notable changes to this project will be documented in this file.

## [Unreleased]
### Changed
- Optimization of the algorithm for receiving data. Fix bug with clearing the buffer the received data timeout
- Fix bug with the lack of responses from the device when connecting.

## [1.0.1] - 2019-04-xx
### Changed
- Change some syntax for limits into xml config.
- Add SignedCommand subclass for Command. It uses for display signed values.
- When user stop the device, the software use an extra delay. It didn't occure when user stop the TEC. This problem was fixed. TEC stopping is also going with extra delay.
### Fixed
- incorrect display of current value due to divider error. Change xml config. Dividers use only for "command" tags.
- Each time you connect to device, if it returns a "common ID", user is prompted to select a custom device model.
- Limit settings window is working correctly now. Fix bug with sending new limits without code: user could send a new limit like "P <value>".
- TEC button did not show the TEC status.
