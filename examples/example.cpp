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
