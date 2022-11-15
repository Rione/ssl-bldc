import numpy as np
import matplotlib.pyplot as plt
# ゲートソース電圧
Vgs = 15.5

# 寄生ゲート抵抗 1.1Ω
Rgp = 1.2

# 追加するゲート抵抗
Rg = 50  # [10, 20, 50, 100]

# Total gate charge 20nC
Qg = 20e-9

# Gate-source charge 2.7nC
Qgs = 2.7e-9

# Gate-drain charge 1.8nC
Qgd = 1.8e-9

# Ciss 795pF
Ciss = 795e-12

# Q = CV → C = Q/V
Cg = Qg / Vgs  # これであってるん？Cgってここで書きたいのはトータル電荷量ではなくCissじゃないの？
print('Cg:', '{:.6g}'.format(Cg * 1e12), 'pF')

# 時定数　τ = RC
tau = Ciss * (Rgp + Rg)

# 最大ゲート電流
Ig = Vgs / (Rgp + Rg)
print('IgMax:', '{:.3g}'.format(Ig), 'A')

# 3τ後のゲート電流z
Ig_3tau = Ig*(1-0.95)

# 駆動周波数 KHz
F = 20e3
# 周期 us
T = 1 / F
print('period:', T * 1e9, 'ns')

T_min = T / 256
print('period Min:', T_min * 1e9, 'ns')

# ターンオン時間
timeTurnOn = -tau * np.log(Ig_3tau * (Rgp + Rg) / Vgs)
print('timeTurnOn', '{:.3g}'.format(timeTurnOn * 1e9), 'ns')


# Graph Settings
fig = plt.figure()  # グラフ領域作成
graph1 = fig.add_subplot(1, 2, 1)  # グラフ1
graph2 = fig.add_subplot(1, 2, 2)  # グラフ1
t = np.linspace(0.0000000001,  8 * tau, 200)  # 時間ベクトル作成


def transientCurrent(t):  # 過渡電流
    return Vgs / (Rgp + Rg) * (np.exp(-t / tau))


def drawGateResistorCurrentGraph():  # ゲート抵抗に流れる電流のグラフ
    graph1.set_xlabel('t')
    graph1.set_ylabel('i(t)')
    graph1.set_title('Gate Resistor Current [' + 'Rg:' + str(Rg) + 'Ω' +
                     ' ,Vgs:' + str(Vgs) + 'V]')
    graph1.plot(t, transientCurrent(t))
    graph1.plot(tau * 1, transientCurrent(tau * 1), marker='.')  # 1τ
    graph1.plot(tau * 2, transientCurrent(tau * 2), marker='.')  # 2τ
    graph1.plot(tau * 3, transientCurrent(tau * 3), marker='.')  # 3τ


def transientCVoltage(t):  # コンデンサの過渡電圧
    return Vgs * (1-np.exp(-t / tau))


def drawCissVoltageGraph():  # Cissにチャージされた電圧のグラフ
    graph2.set_xlabel('t')
    graph2.set_ylabel('V(t)')
    graph2.set_title('Ciss Voltage [' + 'Rg:' + str(Rg) + 'Ω' +
                     ' ,Vgs:' + str(Vgs) + 'V]')
    graph2.plot(t, transientCVoltage(t))
    graph2.plot(tau * 1, transientCVoltage(tau * 1), marker='.')  # 1τ
    graph2.plot(tau * 2, transientCVoltage(tau * 2), marker='.')  # 2τ
    graph2.plot(tau * 3, transientCVoltage(tau * 3), marker='.')  # 3τ


def main():
    print('Param [Rg:', Rg, 'Ω', 'Vgs:', Vgs, 'V]')
    drawGateResistorCurrentGraph()  # ゲート抵抗に流れる電流のグラフ
    drawCissVoltageGraph()  # Cissにチャージされた電圧のグラフ
    plt.show()


if __name__ == "__main__":
    main()
