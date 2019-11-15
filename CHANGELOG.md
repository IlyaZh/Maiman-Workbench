# Changelog
All notable changes to this project will be documented in this file.

## [Unreleased]

## [1.0.4] - 2019-11-15
### Fixed
- Signed commands issue. Sign wasn't shown
- Fix change temoearute symbol bug. Units doesn't change when user change Celsius\Farenheit units, only values recalculates before. 

## [1.0.3] - 2019-10-29
### Added
- Showing suestion message about laser state if user tries to close an app or disconnect when the laser on
- Checkboxes saved when user change its value and load when user connects to device
### Changed
- Refactoring of some issues
- Optimization of the algorithm for receiving data. Fix bug with clearing the buffer the received data timeout
- Fix bug with the lack of responses from the device when connecting.

## [1.0.2] - 2019-08-22
### Added
- Add abilitiy to save state of device by one click and then load it again. It makes due to device status not saved when the device restarted.
- Safety fix. If the laser started and you exit the software it will try to stop laser.

## [1.0.1] - 2019-04-20
### Changed
- Change some syntax for limits into xml config.
- Add SignedCommand subclass for Command. It uses for display signed values.
- When user stop the device, the software use an extra delay. It didn't occure when user stop the TEC. This problem was fixed. TEC stopping is also going with extra delay.
### Fixed
- incorrect display of current value due to divider error. Change xml config. Dividers use only for "command" tags.
- Each time you connect to device, if it returns a "common ID", user is prompted to select a custom device model.
- Limit settings window is working correctly now. Fix bug with sending new limits without code: user could send a new limit like "P <value>".
- TEC button did not show the TEC status.
- Fixed an issue with displaying the measured value. If measured value is over then bar will not show new value.
