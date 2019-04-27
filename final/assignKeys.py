# Assign unique keys to each of the 39 characters input array
#     using a prime hash style function. In our C program
#     we'll have two arrays with a universal index, in combination
#     function like a dictionary.

# Script used to quickly test prime values, to experiement in
#     finding a unique key set. Since results are for two 
#     hardcoded arrays in our main.c, they can also be printed
#     for convenience.

import csv
from collections import Counter

keys = []
mapping = []
allCombos = []
primes  = [5,7,13,17,19,23] # Generates unique list of keys to pair

# https://stackoverflow.com/questions/5278122/checking-if-all-elements-in-a-list-are-unique
def isUnique(array):
  result = dict(Counter(array)).values()
  print(result)
  for i in result:
    if i is not 1:
      return False

def printKeysForC():
  result = "int keys[39] = {"
  for eachTuple in mapping:
    result += (str(eachTuple[1]) + ",")
  result = result[:-1] + '};'
  print(result)

def printCharsForC():
  result = "char chars[39] = {"
  for eachTuple in mapping:
    result += str("'" + str(eachTuple[0]) + "',")
  result = result[:-1] + '};'
  print(result)

def processCSV():
  with open('newEncoding.csv') as csvFile:
    csvReader = csv.reader(csvFile, delimiter=',')
    lineCount = 0
    for row in csvReader:
      if (lineCount != 0):
        uniqueSum = 0
        for i in range(0,6):
          uniqueSum += primes[i]*int(str(row[2])[i])
        keys.append(uniqueSum)
        mapping.append( (row[0], uniqueSum) )
      lineCount += 1

def hashAllCombos():
  for possibleInputSet in allCombos:
    hashOutput = 0
    for i in range(0,6):
        hashOutput += int(possibleInputSet[i]) * primes[i]
    keys.append(hashOutput)

def getAllCombos():
  for a in range(0,3):
    if a is not 0:
      for b in range(0,3):
        if b is not 0:
          for c in range(0,3):
            if c is not 0:
              for d in range(0,3):
                if d is not 0:
                  for e in range(0,3):
                    if e is not 0:
                      for f in range(0,3):
                        allCombos.append(str(a)+str(b)+str(c)+str(d)+str(e)+str(f))

#getAllCombos()
#print(len(allCombos))
getAllCombos()
hashAllCombos()
print("Keys are Unique? - "+str(isUnique(keys)))
#printCharsForC()
#printKeysForC()