import numpy as np
import matplotlib.pyplot as plt

#ゲート入力電荷量 4.65nC
Qg = 4.65e-9

#ゲートスイッチ電荷量 6.4nC
Qs = 6.4e-9

#ゲートソース電圧
Vgs = 16.4

#ゲート抵抗 1.1Ω
Rg = 1.1

#ゲート入力容量F
Cg = Qg / Vgs + Qs / Vgs

t = np.linspace(0.0000000001,  4 * Rg * Cg, 100)
def tau(x):
    return x * Cg * Rg


def func(t):
    return Vgs * (1 - np.exp(-t / (Rg * Cg)))


plt.plot(t, func(t))
plt.plot(tau(1), func(tau(1)), marker='.')
plt.plot(tau(2), func(tau(2)), marker='.')
plt.plot(tau(3), func(tau(3)), marker='.')
# plt.plot(tau(4), func(tau(4)), marker='.')
# plt.plot(tau(5), func(tau(5)), marker='.')
# plt.plot(tau(6), func(tau(6)), marker='.')
plt.xlabel('t')
plt.ylabel('E[V]')
# plt.show()


t = np.linspace(0.0000000001,  4 * Rg * Cg, 100)


def i(t):
    return Vgs / (Rg * (1 - np.exp(-t / (Rg * Cg))))


plt.plot(t, i(t))

plt.xlabel('t')
plt.ylabel('i(t)')

# plt.plot(0.5 * Cg * Rg, i(0.5 * Cg * Rg), marker='.')
# plt.plot(0.25 * Cg * Rg, i(0.25 * Cg * Rg), marker='.')
# plt.plot(0.125 * Cg * Rg, i(0.125 * Cg * Rg), marker='.')
plt.plot(Cg *Rg, i(Cg * Rg), marker='.')
plt.show()