#include <adc/adc.hpp>
#include "../examples/fake.hpp"

#include <cassert>

using namespace mhaa::adc;
using namespace example;

class ImmediateConverter final : public Converter
{
public:
    Status start(ChannelId channel) override
    {
        if (channel != ChannelId{})
        {
            return Status::InvalidChannel;
        }
        if (unread_)
        {
            return Status::Busy;
        }
        unread_ = true;
        return Status::Success;
    }

    Status try_read(Sample &sample) override
    {
        if (!unread_)
        {
            return Status::NoSample;
        }
        sample = 900;
        unread_ = false;
        return Status::Success;
    }

    uint8_t resolution_bits() const override { return 10; }

private:
    bool unread_ = false;
};

class FakeReference final : public ReferenceControl, public TriggerControl
{
public:
    explicit FakeReference(const Fake &converter) : converter_(converter) {}

    Status set_reference(ReferenceSelection selection) override
    {
        if (converter_.pending)
        {
            return Status::Busy;
        }
        if (selection.kind != Reference::Internal || selection.id != static_cast<ReferenceId>(1))
        {
            return Status::Unsupported;
        }
        selected_ = true;
        return Status::Success;
    }

    bool nominal_reference_uv(uint32_t &voltage_uv) const override
    {
        if (!selected_)
        {
            return false;
        }
        voltage_uv = 2048000;
        return true;
    }

    Status set_trigger(TriggerSource trigger) override
    {
        return trigger.kind == TriggerKind::Software && trigger.source_id == TriggerId{}
                   ? Status::Success
                   : Status::Unsupported;
    }

private:
    const Fake &converter_;
    bool selected_ = false;
};

int main()
{
    Fake low_resolution(8, 200);
    Fake high_resolution(24, 80000);
    Converter *converters[] = {&low_resolution, &high_resolution};
    for (Converter *converter : converters)
    {
        ADC adc(*converter);
        Sample sample = 7;
        assert(adc.start(static_cast<ChannelId>(1)) == Status::InvalidChannel);
        assert(adc.try_read(sample) == Status::NoSample && sample == 7);
        assert(adc.start(ChannelId{}) == Status::Success);
        assert(adc.start(ChannelId{}) == Status::Busy);
        assert(adc.try_read(sample) == Status::Busy && sample == 7);
    }
    low_resolution.ready = high_resolution.ready = true;
    ADC low(low_resolution);
    ADC high(high_resolution);
    Sample sample = 0;
    assert(low.resolution_bits() == 8 && high.resolution_bits() == 24);
    assert(low.try_read(sample) == Status::Success && sample == 200);
    assert(high.try_read(sample) == Status::Success && sample == 80000);
    assert(high.start(ChannelId{}) == Status::Success);
    high_resolution.failed = true;
    assert(high.try_read(sample) == Status::Error && sample == 80000);
    assert(high.try_read(sample) == Status::NoSample);

    ImmediateConverter immediate;
    Converter &compatible = immediate;
    ADC immediate_adc(compatible);
    assert(immediate_adc.start(ChannelId{}) == Status::Success);
    assert(immediate_adc.start(ChannelId{}) == Status::Busy);
    assert(immediate_adc.try_read(sample) == Status::Success && sample == 900);
    assert(immediate_adc.try_read(sample) == Status::NoSample);

    FakeReference references(high_resolution);
    ADC adjustable(high_resolution, &references, &references);
    const ReferenceSelection alternate{Reference::Internal, static_cast<ReferenceId>(1)};
    uint32_t voltage_uv = 1;
    assert(immediate_adc.set_reference(alternate) == Status::Unsupported);
    assert(!immediate_adc.nominal_reference_uv(voltage_uv) && voltage_uv == 1);
    assert(adjustable.set_reference(alternate) == Status::Success);
    assert(adjustable.nominal_reference_uv(voltage_uv) && voltage_uv == 2048000);
    assert(adjustable.set_trigger({TriggerKind::Timer, TriggerId{}}) == Status::Unsupported);
    assert(adjustable.set_trigger({TriggerKind::Software, TriggerId{}}) == Status::Success);
    assert(adjustable.start(ChannelId{}) == Status::Success);
    assert(adjustable.set_reference(alternate) == Status::Busy);
}
