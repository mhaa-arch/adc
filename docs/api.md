# Use the ADC Core

Public entry headers are [adc/adc.hpp](../include/adc/adc.hpp).

## Schedule one sample

Construct `ADC` from a live `Converter&`. Check `start(channel)` before scheduling a poll. A scheduler can do other work while `try_read(sample)` reports `Busy`. On `Success` consume the raw value. On `NoSample` there is no pending result. Treat a terminal error as the end of that conversion. `resolution_bits()` describes the converter's code width.

## Bind optional controls

Pass `ReferenceControl*` and `TriggerControl*` for the same converter when you construct the Core. A missing setter returns `Unsupported`. A missing or unknown reference voltage produces false and leaves the supplied value unchanged. Configure before starting, and account for any required settling time. The Core delegates conversion, so voltage conversion, calibration and scheduling belong to the caller.

## Composition and lifetime

Application code calls the ADC Core. It borrows the matching Contract, which an Adapter implements using its Hardware Definitions. Future Devices will create or bind these objects and establish their lifetime. To use the examples today, construct the facade directly with the supplied fake. Keep each borrowed resource alive for its use by the facade and follow the Adapter's access and synchronization requirements.
