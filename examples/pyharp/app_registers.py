"""App registers for the cuttlefish-fip controller."""
from enum import IntEnum


class AppRegs(IntEnum):
    PWMState = 32
    PWMFreq = 33
    PWMDutyCycle = 34
