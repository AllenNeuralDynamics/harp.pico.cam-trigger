#ifndef PWM_H
#define PWM_H
#include <stdint.h>
#include <pico/stdlib.h>
#include <hardware/pwm.h>
#include <hardware/clocks.h>
#include <cmath>
#ifdef DEBUG
    #include <cstdio> // for printf
#endif


/**
 * \brief class for motor controller with (Enable Pin, Direction Pin) interface.
 */
class PWM
{
public:
    /**
     * \brief constructor. Connect to PWM hardware, set set pwm_pin to output 0.
     * \param pwm_pin the pwm output pin to the controller enable pin.
     */
    PWM(uint8_t pwm_pin);

    /**
     * \brief destructor. Leave the outputs off and configured as input.
     */
    ~PWM();

    /**
     * \brief set the pwm frequency in Hz.
     * \returns the actual pwm frequency set on the device.
     */
    float set_frequency(float freq_hz);

    /**
     *\brief set the duty cycle in Hz.
     * \returns the actual duty cycle set on the device.
     */
    float set_duty_cycle(float normalized_duty_cycle);

    /**
     * \brief enable the pwm output
     * \note inline
     */
    inline void enable_output()
    {
        pwm_enabled_ = true;
        pwm_set_chan_level(slice_num_, gpio_channel_, duty_cycle_);
    }

    /**
     * \brief disable the pwm output by setting duty cycle to 0.
     * \note inline.
     */
    inline void disable_output()
    {
        // We disable by setting the duty cycle to zero because disabling the
        // PWM peripheral leaves the GPIO pin in the previous state, which
        // could be 1.
        pwm_enabled_ = false;
        pwm_set_chan_level(slice_num_, gpio_channel_, 0);
    }

private:
    uint pwm_pin_;
    uint slice_num_;
    uint gpio_channel_;
    uint duty_cycle_; /// The current duty cycle setting.
    bool pwm_enabled_;

    // Constants
    static const uint16_t PWM_STEP_INCREMENTS = 50000;
    static const uint DEFAULT_PWM_FREQ_HZ = 200;
};

#endif // PWM_H
