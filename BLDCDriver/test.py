import numpy as np
import matplotlib.pyplot as plt

# ゲート入力電荷量 4.65nC
Qg = 4.65e-9

# ゲートスイッチ電荷量 6.4nC
Qs = 6.4e-9

# ゲートソース電圧
Vgs = 16.4

# 寄生ゲート抵抗 1.1Ω
Rs = 1.1

# 追加するゲート抵抗
Rg = 30

# ゲート抵抗の和
R = Rg + Rs

# ゲート入力容量F
Cg = Qg / Vgs + Qs / Vgs

t = np.linspace(0.000000001,  6 * R * Cg, 100)


def tau(x):
    return x * Cg * R


def func(t):
    return Vgs * (1 - np.exp(-t / (R * Cg)))


plt.plot(t, func(t))
plt.plot(tau(1), func(tau(1)), marker='.')  # 1τ
plt.plot(t, func(t))
plt.plot(tau(3), func(tau(3)), marker='.')  # 1τ
plt.xlabel('t')
# plt.ylabel('i(t)')


def i(t):
    return Vgs / (R * (1 - np.exp(-t / (R * Cg))))


plt.plot(t, i(t))

plt.xlabel('t')
# plt.ylabel('i(t)')
plt.plot(1 * Cg * R, i(1 * Cg * R), marker='.')
plt.plot(3 * Cg * R, i(3 * Cg * R), marker='.')


I = i(1 * Cg * R)
T = 1 * Cg * R * 1e9  # ns
# print Current
print('Current' + '{:.4g}'.format(I), "A")
# print time ns
print('{:.4g}'.format(T), "ns")

plt.show()
