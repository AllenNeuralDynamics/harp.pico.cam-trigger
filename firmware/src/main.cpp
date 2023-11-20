#include <cstring>
#include <harp_c_app.h>
#include <harp_synchronizer.h>
#include <core_registers.h>
#include <reg_types.h>
#ifdef DEBUG
    #include <pico/stdlib.h> // for uart printing
    #include <cstdio> // for printf
#endif
#include <pwm.h>
#include <config.h>


// Create PWM instance.
PWM pwm_{PWM_PIN};

// Create device name array.
const uint16_t who_am_i = 1234;
const uint8_t hw_version_major = 1;
const uint8_t hw_version_minor = 0;
const uint8_t assembly_version = 2;
const uint8_t harp_version_major = 2;
const uint8_t harp_version_minor = 0;
const uint8_t fw_version_major = 3;
const uint8_t fw_version_minor = 0;
const uint16_t serial_number = 0xCAFE;

// Harp App Register Setup.
const size_t reg_count = 3;

// Define register contents.
#pragma pack(push, 1)
struct app_regs_t
{
    volatile uint8_t pwm_state;  // app register 0
    volatile float pwm_freq; // app register 1
    volatile float duty_cycle; // app register 2
} app_regs;
#pragma pack(pop)

// Define register "specs."
RegSpecs app_reg_specs[reg_count]
{
    {(uint8_t*)&app_regs.pwm_state, sizeof(app_regs.pwm_state), U8},
    {(uint8_t*)&app_regs.pwm_freq, sizeof(app_regs.pwm_freq), Float},
    {(uint8_t*)&app_regs.duty_cycle, sizeof(app_regs.duty_cycle), Float}
};

// Create reg handler functions.
void set_pwm_state(msg_t& msg)
{
    app_regs.pwm_state = msg.payload_as_uint8();
    if (app_regs.pwm_state)
        pwm_.enable_output();
    else
        pwm_.disable_output();
    HarpCore::write_reg_generic(msg);
}

void set_pwm_frequency_hz(msg_t& msg)
{
    float& frequency_hz = msg.payload_as_float();
    app_regs.pwm_freq = pwm_.set_frequency(frequency_hz);
    HarpCore::write_reg_generic(msg);
}

// Set duty cycle as a float from 0.0 to 1.0
void set_duty_cycle_percentage(msg_t& msg)
{
    float& normalized_duty_cycle = msg.payload_as_float();
    app_regs.duty_cycle = pwm_.set_duty_cycle(normalized_duty_cycle);
    HarpCore::write_reg_generic(msg);
}

// Define register read-and-write handler functions.
RegFnPair reg_handler_fns[reg_count]
{
    {&HarpCore::read_reg_generic, &set_pwm_state},
    {&HarpCore::read_reg_generic, &set_pwm_frequency_hz},
    {&HarpCore::read_reg_generic, &set_duty_cycle_percentage}
};


void app_reset()
{
    app_regs.pwm_state = 0;
    app_regs.pwm_freq = pwm_.set_frequency(200.0f); // Set a default frequency.
    app_regs.duty_cycle = pwm_.set_duty_cycle(0.5f);
    pwm_.disable_output();
}

void update_app_state()
{
    // update here!
    // If app registers update their states outside the read/write handler
    // functions, update them here.
    // (Called inside run() function.)
}

// Create Harp App.
HarpCApp& app = HarpCApp::init(who_am_i, hw_version_major, hw_version_minor,
                               assembly_version,
                               harp_version_major, harp_version_minor,
                               fw_version_major, fw_version_minor,
                               serial_number, "Cam Trigger",
                               &app_regs, app_reg_specs,
                               reg_handler_fns, reg_count, update_app_state,
                               app_reset);

// Core0 main.
int main()
{
// Init Synchronizer.
    HarpSynchronizer& sync = HarpSynchronizer::init(uart1, SYNC_PIN);
#ifdef DEBUG
    stdio_uart_init_full(uart0, 921600, 0, -1); // use uart1 tx only.
    printf("Hello, from an RP2040!\r\n");
#endif
    app_reset();
    while(true)
    {
        app.run();
    }
}
