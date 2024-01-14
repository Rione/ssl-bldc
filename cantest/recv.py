import can
from serial.tools.list_ports import comports
import colorama
from colorama import Fore
from time import sleep


colorama.init(autoreset=True)


def port_serch(text):
    ports = []
    for n, (port, desc, devid) in enumerate(sorted(comports()), 1):
        print(f" {n}: {port:20} {desc} {devid}")
        ports.append(port)
        if text in desc:

            print(f"{Fore.CYAN}'{text}' found on port: "+port)
            return port


def receive_can_messages(bus):
    message = bus.recv(timeout=1.0)
    if message is not None:
        if message.is_rx:
            color = Fore.CYAN
        else:
            color = Fore.RED
        print(f"{color}{message}")


if __name__ == '__main__':
    serial_port = port_serch("CANable")
    bus = can.interface.Bus(
        bustype='slcan', channel=serial_port, bitrate=1000000)

    try:
        while True:
            receive_can_messages(bus)
    except KeyboardInterrupt:
        pass
    finally:
        print(f"{Fore.RED}Shutting down.")
        bus.shutdown()
