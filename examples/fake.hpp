#ifndef MHAA_ADC_EXAMPLE_FAKE_HPP
#define MHAA_ADC_EXAMPLE_FAKE_HPP

#include <adc/converter.hpp>
#include <adc/reference_control.hpp>
#include <adc/trigger_control.hpp>

namespace example
{
    using namespace mhaa::adc;

    class Fake final : public Converter
    {
    public:
        Fake(uint8_t bits, Sample value) : bits_(bits), value_(value) {}
        Status start(ChannelId channel) override
        {
            if (channel != ChannelId{})
            {
                return Status::InvalidChannel;
            }
            if (pending)
            {
                return Status::Busy;
            }
            pending = true;
            return Status::Success;
        }
        Status try_read(Sample &sample) override
        {
            if (!pending)
            {
                return Status::NoSample;
            }
            if (!ready)
            {
                return Status::Busy;
            }
            pending = false;
            if (failed)
            {
                return Status::Error;
            }
            sample = value_;
            return Status::Success;
        }
        uint8_t resolution_bits() const override { return bits_; }

        bool pending = false;
        bool ready = false;
        bool failed = false;

    private:
        uint8_t bits_;
        Sample value_;
    };
}

#endif
