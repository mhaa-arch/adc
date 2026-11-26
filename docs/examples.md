# Run the ADC Core example

The 12-bit fake is constructed before `ADC`, so it outlives the Core. Start logical channel zero, inject completion by setting the fake's ready flag, then collect an unsigned sample of 3072. On hardware, completion comes from the implementation and a pending poll is a normal outcome. The example checks status before consuming the sample.

## Example program

The [program](../examples/example.cpp) uses the [local fake](../examples/fake.hpp) to demonstrate injection. A concrete Adapter implementing the same Contract can be supplied to the same facade.

```cpp
#include <adc/adc.hpp>
#include "fake.hpp"

using namespace mhaa::adc;
using namespace example;

int main()
{
    Fake converter(12, 3072);
    ADC adc(converter);
    Sample sample = 0;
    if (adc.start(ChannelId{}) != Status::Success)
    {
        return 1;
    }
    converter.ready = true;
    return adc.try_read(sample) == Status::Success && sample == 3072 ? 0 : 1;
}
```

## Build and run

Build from this repository root. Set `contract_include` to the matching Contract's `include/` directory. The value below applies to the MHAA development workspace. Both programs exit with status 0 on success and print no output. On Windows execute `example.exe` and `behavior.exe`.

```sh
contract_include=../../contracts/adc-contract/include
g++ -std=c++11 -Wall -Wextra -Wpedantic -Werror -fno-rtti -fno-exceptions -Iinclude -I"$contract_include" examples/example.cpp -o example
./example
g++ -std=c++11 -Wall -Wextra -Wpedantic -Werror -fno-rtti -fno-exceptions -Iinclude -I"$contract_include" tests/behavior.cpp -o behavior
./behavior
```

See the [behavior test](../tests/behavior.cpp) for the additional cases described above.
