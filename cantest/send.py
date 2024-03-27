from time import sleep

import can
import colorama
from colorama import Fore
from serial.tools.list_ports import comports

colorama.init(autoreset=True)


def port_serch(text):
    ports = []
    for n, (port, desc, devid) in enumerate(sorted(comports()), 1):
        print(f" {n}: {port:20} {desc} {devid}")
        ports.append(port)
        if text in desc:
            print(f"{Fore.GREEN}'{text}' found on port: "+port)
            return port
    print(f"{Fore.RED}'{text}' not found.")
    return None


def receive_can_messages(bus):
    message = bus.recv(timeout=0.1)
    if message is not None:
        if message.is_rx:
            color = Fore.CYAN
        else:
            color = Fore.RED

        print(
            f"{color}RX:{'E' if message.is_error_frame else ' '} ID:{message.arbitration_id:04x}  data:{message.data[0]:02x} {message.data[1]:02x} {message.data[2]:02x} {message.data[3]:02x} {message.data[4]:02x} {message.data[5]:02x} {message.data[6]:02x} {message.data[7]:02x}")


if __name__ == '__main__':
    serial_port = port_serch("CANable")
    if serial_port is None:
        exit()
    bus = can.interface.Bus(
        bustype='slcan', channel=serial_port, bitrate=1000000)

    # message = can.Message(arbitration_id=0x123, data=[
    #     0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88], is_extended_id=False)
    message = can.Message(arbitration_id=0x00, is_extended_id=False)
    vel = 0
    a = 1
    try:
        while True:
            try:
                vel_l = int(vel & 0xFF)
                vel_h = int((vel >> 8) & 0xFF)

                message = can.Message(arbitration_id=0x1AA, data=[vel_l, vel_h, 0, 0, 0, 0, 0, 0], is_extended_id=False)

                if vel >= 250 or vel <= -250:
                    a = -a

                vel += a

                bus.send(message)  # ここでメッセージを送信しています。
                print(
                    f"{Fore.RED}TX:{'E' if message.is_error_frame else ' '} ID:{message.arbitration_id:04x} Vel:{vel} data:{message.data[0]:02x} {message.data[1]:02x} {message.data[2]:02x} {message.data[3]:02x} {message.data[4]:02x} {message.data[5]:02x} {message.data[6]:02x} {message.data[7]:02x}")
            except can.CanError:
                print("Message NOT sent")

            # receive_can_messages(bus)  # 受信したデータを表示
            sleep(0.02)  # 0.1秒待ち
    except KeyboardInterrupt:
        pass
    finally:
        print(f"{Fore.GREEN}Shutting down.")
        bus.shutdown()
