import asyncio
from bleak import BleakScanner
from bleak.backends.device import BLEDevice
from bleak.backends.scanner import AdvertisementData
import logging
import ctypes

logging.basicConfig()


def simple_callback(device: BLEDevice, advertisement_data: AdvertisementData):
    if device.address == "E1:38:D4:CD:DE:AF":
        rssi_list = []
        us_list = []
        adv_bytearray = list(advertisement_data.service_data.values())[0]
        node_values = [ctypes.c_int8(adv_bytearray[i]).value for i in range(len(adv_bytearray))]
        rssi_list.extend(node_values[3:15])
        us_list.extend(node_values[15:17])
        print("RSSI: ", rssi_list, ",  US: ", us_list)


async def run():
    scanner = BleakScanner()
    scanner.register_detection_callback(simple_callback)

    while True:
        await scanner.start()
        await asyncio.sleep(0.03)
        await scanner.stop()


if __name__ == "__main__":
    loop = asyncio.get_event_loop()
    loop.run_until_complete(run())
