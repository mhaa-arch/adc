# Using ADC across targets

The only project dependency is the matching `adc-contract`. Keep its API revision aligned with this Core.

Applications can preserve their start/poll code when channel mappings or resolution change. Before reusing scaling or threshold logic, check the selected reference, its tolerance and the effective resolution. Treat a pending hardware trigger separately from a software scheduling deadline. The Core supplies no wait or trigger timeout.

This repository's host simulations and shared API compile/link checks provide source-level evidence. Target timing and physical behavior require hardware validation.
