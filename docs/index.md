# ADC Core guide

Starting, completing and consuming a conversion are separate events. In particular, a completed but unread sample still occupies the converter.

Start with the conversion lifecycle, then read how reference and trigger routes are named.

- [Use the facade](api.md) for construction, results and optional capabilities.
- [Run the example](examples.md) to see the same API with a simulated resource.
- [Check portability](compatibility.md) for assumptions that belong to the target.
