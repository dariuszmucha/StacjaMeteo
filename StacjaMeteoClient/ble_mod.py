import sys
import os
import struct
from ctypes import (CDLL, get_errno)
from ctypes.util import find_library
from socket import (
    socket,
    AF_BLUETOOTH,
    SOCK_RAW,
    BTPROTO_HCI,
    SOL_HCI,
    HCI_FILTER,
)
import time

class ble_mod():

    sock = 0
    bluez = 0
    
    _temperature
    _humidity
    _pm10
    _pm25        
    
    def __init__(self):
        if not os.geteuid() == 0:
            sys.exit("script only works as root")

        btlib = find_library("bluetooth")
        if not btlib:
            raise Exception(
                "Can't find required bluetooth libraries"
                " (need to install bluez)"
            )
        self.bluez = CDLL(btlib, use_errno=True)

        dev_id = self.bluez.hci_get_route(None)

        self.sock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)
        self.sock.bind((dev_id,))

    def start_scan(self):
        err = self.bluez.hci_le_set_scan_parameters(self.sock.fileno(), 0, 0x10, 0x10, 0, 0, 1000);
        if err < 0:
                self.stop_scan()
                raise Exception("Set scan parameters failed" + str(err))
            # occurs when scanning is still enabled from previous call

        # allows LE advertising events
        hci_filter = struct.pack(
            "<IQH",
            0x00000010,
            0x4000000000000000,
            0
        )
        self.sock.setsockopt(SOL_HCI, HCI_FILTER, hci_filter)

        err = self.bluez.hci_le_set_scan_enable(
            self.sock.fileno(),
            1,  # 1 - turn on;  0 - turn off
            0, # 0-filtering disabled, 1-filter out duplicates
            1000  # timeout
        )
        if err < 0:
            raise Exception("Start scan failed")

    def stop_scan(self):
        err = self.bluez.hci_le_set_scan_enable(
            self.sock.fileno(),
            0,  # 1 - turn on;  0 - turn off
            0, # 0-filtering disabled, 1-filter out duplicates
            1000  # timeout
        )
        if err < 0:
            raise Exception("Stop scan failed")

    def get_devices(self):
        while True:
            data = self.sock.recv(1024)
            #print(':'.join("{0:02x}".format(x) for x in data[15:21]))
            time.sleep(2)
            print("Temperature = " + str(data[17]) + "C")
            print("Humidiry = " + str(data[16]) + "%")
            rssi = data[-1]
            if(rssi > 127):
                    rssi = (256 - rssi)*(-1)
            print("RSSI = %d dB" % rssi)