#ifndef MHAA_ADC_HPP
#define MHAA_ADC_HPP

#include <stdint.h>
#include <adc/converter.hpp>
#include <adc/reference_control.hpp>
#include <adc/trigger_control.hpp>
#include <adc/types.hpp>

namespace mhaa
{
    namespace adc
    {
        /** @brief Starts and polls raw conversions on one borrowed converter. */
        class ADC
        {
        public:
            explicit ADC(Converter &converter, ReferenceControl *reference = nullptr,
                         TriggerControl *trigger = nullptr)
                : converter_(converter), reference_(reference), trigger_(trigger) {}

            Status start(ChannelId channel) { return converter_.start(channel); }
            Status try_read(Sample &sample) { return converter_.try_read(sample); }
            uint8_t resolution_bits() const { return converter_.resolution_bits(); }

            Status set_reference(ReferenceSelection reference)
            {
                return reference_ ? reference_->set_reference(reference) : Status::Unsupported;
            }

            bool nominal_reference_uv(uint32_t &voltage_uv) const
            {
                return reference_ && reference_->nominal_reference_uv(voltage_uv);
            }

            Status set_trigger(TriggerSource source)
            {
                return trigger_ ? trigger_->set_trigger(source) : Status::Unsupported;
            }

        private:
            Converter &converter_;
            ReferenceControl *reference_;
            TriggerControl *trigger_;
        };
    }
}

#endif
