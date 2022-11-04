import numpy as np
import matplotlib.pyplot as plt

# ゲート入力電荷量 4.65nC
Qg = 4.65e-9

# ゲートスイッチ電荷量 6.4nC
Qs = 6.4e-9

# ゲートソース電圧
Vgs = 15

# 寄生ゲート抵抗 1.1Ω
Rs = 1.1

# 追加するゲート抵抗
Rg = 50

# Ciss 2000pF
Ciss = 2000e-12
# ゲート抵抗の和
R = Rg + Rs

# ゲート入力容量F
# Cg = Qg / Vgs + Qs / Vgs
Cg = Ciss  # alpacoにこれくらいになるって言われた

t = np.linspace(0.0000000001,  6 * R * Cg, 200)


def tau(x):
    return x * Cg * R


def func(t):
    return Vgs * (1 - np.exp(-t / (R * Cg)))


def i(t):
    return Vgs / (R * (1 - np.exp(-t / (R * Cg))))


# plt.plot(t, i(t))

# plt.xlabel('t')
# # plt.ylabel('i(t)')
# plt.plot(1 * Cg * R, i(1 * Cg * R), marker='.')
# plt.plot(2 * Cg * R, i(3 * Cg * R), marker='.')


I = i(1 * Cg * R)
T = 1 * Cg * R * 1e9  # ns


plt.plot(t, func(t))
plt.plot(tau(1), func(tau(1)), marker='.')  # 1τ
plt.text(tau(1), func(tau(1)) + 0.1, '1τ:'+'{:.4g}'.format(T) + 'ns')

plt.plot(tau(2), func(tau(2)), marker='.')  # 2τ
plt.text(tau(2), func(tau(2)) + 0.1, '2τ:'+'{:.4g}'.format(T * 2) + 'ns')

plt.plot(tau(3), func(tau(3)), marker='.')  # 3τ
plt.text(tau(3), func(tau(3)) + 0.1, '3τ:'+'{:.4g}'.format(T * 3) + 'ns')
plt.xlabel('t')
# plt.ylabel('i(t)')

plt.title('Gate resistor:' + str(Rg) + 'Ω' +
          ' ,Gate source voltage:' + str(Vgs) + 'V' + ' ,Current:' + '{:.4g}'.format(I) + 'A')

# plt.plot(3 * Cg * R, i(3 * Cg * R), marker='.')


# print Current
print('Current' + '{:.4g}'.format(I), "A")
# print time ns
print('1tau:' + '{:.4g}'.format(T), "ns")
print('2tau:' + '{:.4g}'.format(T * 2), "ns")
print('3tau:' + '{:.4g}'.format(T * 3), "ns")


plt.show()
