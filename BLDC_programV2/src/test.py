# NOTE:これ動いてないです！！！てへぺろ!!
import serial
import numpy as np

# シリアルポートの設定（'COM1'を実際のポート名に変更）
ser = serial.Serial('/dev/cu.usbmodem2103', 230400)  # ボーレートは適宜変更

# ヘッダバイトを送信
header_byte = bytes([0xAA])
ser.write(header_byte)

# 速度の値 (int16_t) を2バイトにスプリットして送信
vel_values = np.array([-100, -200, 300, 400], dtype=np.int16)

checksum = 0

for v in vel_values:
    # 速度の値を2バイトにスプリット
    vel_bytes0 = v & 0b0000000011111111
    vel_bytes1 = (v & 0b1111111100000000) >> 8

    ser.write(bytes([vel_bytes0]))
    ser.write(bytes([vel_bytes1]))
    print(v, vel_bytes0, vel_bytes1)
    checksum = (vel_bytes0 + vel_bytes1 + checksum) % 256

# チェックサムを計算して送信
ser.write(bytes([checksum]))
print('checksum', checksum)

# シリアルポートをクローズ
ser.close()
