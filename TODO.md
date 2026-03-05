# TMAG5273 TODO

## Status: In Progress

### Bugs to Fix
- **Threshold interrupt not firing on hardware** — example compiles but INT pin on D2 doesn't trigger. Need to debug:
  - Is the threshold value correct? (auto-cal may be setting it wrong)
  - Does the sensor actually assert INT low on threshold? Check with scope
  - May need to check INT_CONFIG_1 register bits more carefully
  - Could be INT pin polarity or open-drain issue

- **triggerConversion() is a workaround** — toggles continuous/standby instead of proper standby trigger. The I2C_RD register mode may be the correct way per datasheet but methods 1-3 all returned stale data in testing.

### Completed
- [x] Library with full register API
- [x] 10 hw_tests (all passing with servo rig, USB current fix)
- [x] hw_tests moved to hw_tests/ subfolder with descriptive names
- [x] Fixed readAngle() bit parsing (was reporting 1/23 of actual angle)
- [x] Added readMagnitudeMT() with auto range conversion
- [x] begin() sets full defaults (XYZ, 32x, continuous, low noise, NdFeB, XY angle, INT pin, data ready)
- [x] 5 examples: simpletest, fulltest, unified_sensor, magnetic_threshold, triggered
- [x] All examples print variant (x1/x2) and actual mT ranges
- [x] CONV_STATUS bit defines added

### Before Release
- [ ] Fix threshold interrupt
- [ ] Fix triggerConversion() properly (understand I2C_RD modes)
- [ ] Push to upstream (adafruit/Adafruit_TMAG5273)
- [ ] Tag release, create GitHub release
- [ ] Submit to arduino library registry
