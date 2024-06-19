
from os import listdir
from os.path import isfile, join
from natsort import os_sorted
import statistics

methods = [f for f in listdir('.')]

for method in methods:
    totalRes = []
    results = [res for res in listdir('./'+method) if isfile('./'+method+'/'+res) and res.startswith("outEval_")]
    results = os_sorted(results)
    for res in results:
        values = []
        file = open("./"+method + '/' + res, 'r')
        lines = file.readlines()
        for line in lines:
            values.append(float(line))
        totalRes.append(values)

    fileOut = open('./'+method + '_total.txt', 'w')
    for j in range(0, 30):
        for i in range(0, 24):
            fileOut.write(str(totalRes[i][j]) + ' ')
        fileOut.write('\n')
    fileOut.write('\n')

    for i in range(0, 24):
        fileOut.write(str(min((totalRes[i]))) + ' ')
    fileOut.write('\n')
    for i in range(0, 24):
        fileOut.write(str(statistics.median((totalRes[i]))) + ' ')
    fileOut.write('\n')
    for i in range(0, 24):
        fileOut.write(str(max((totalRes[i]))) + ' ')
    fileOut.write('\n')
    for i in range(0, 24):
        fileOut.write(str(statistics.mean((totalRes[i]))) + ' ')
    fileOut.write('\n')
    for i in range(0, 24):
        fileOut.write(str(statistics.stdev((totalRes[i]))) + ' ')
    fileOut.write('\n')