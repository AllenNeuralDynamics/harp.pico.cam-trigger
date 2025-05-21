#!/usr/bin/env python3
from pyharp.device import Device, DeviceMode
from pyharp.messages import WriteFloatHarpMessage, WriteU8HarpMessage
from pyharp.messages import MessageType
from pyharp.messages import CommonRegisters as Regs
#import logging
from app_registers import AppRegs
from time import sleep
import serial.tools.list_ports

#logger = logging.getLogger()
#logger.setLevel(logging.DEBUG)
#logger.addHandler(logging.StreamHandler())
#logger.handlers[-1].setFormatter(
#    logging.Formatter(fmt='%(asctime)s:%(name)s:%(levelname)s: %(message)s'))

# Open serial connection with the first camera trigger device
com_port = None
ports = serial.tools.list_ports.comports()
for port, desc, hwid in sorted(ports):
    if desc.startswith("cam-trigger"):
        print("{}: {} [{}]".format(port, desc, hwid))
        com_port = port
        break
device = Device(com_port)

device.send(WriteU8HarpMessage(AppRegs.PWMState, 0).frame)
device.send(WriteFloatHarpMessage(AppRegs.PWMFreq, 20000.).frame)
device.send(WriteFloatHarpMessage(AppRegs.PWMDutyCycle, 0.5).frame)
device.send(WriteU8HarpMessage(AppRegs.PWMState, 1).frame)

while True:
    sleep(0.001)
