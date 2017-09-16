import numpy as np
import matplotlib.pyplot as plt
import math

data = np.genfromtxt('2017-01-20Z14-30-05.steplog', delimiter=',', names=['ts', 'd', 'h', 'i1', 'i2'])

dh = [0.0]
for i in range(len(data['h'])):
    if i == 0:
        continue
    dh.append(data['h'][i] - data['h'][i-1])

x = [0.0]
y = [0.0]
for i in range(len(data['d'])):
    x.append(x[i] + data['d'][i]*math.cos(data['h'][i]))
    y.append(y[i] + data['d'][i]*math.sin(data['h'][i]))

x.pop(0)
y.pop(0)

print type(data)
print data
# plt.hist(data['h'], 100)
# plt.plot(data['ts'], data['h'])
# plt.plot(data['ts'], dh)
# plt.plot(x, y)
plt.plot(data['ts'], y)
plt.show()
