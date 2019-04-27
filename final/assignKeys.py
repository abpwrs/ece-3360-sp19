# Assign unique keys to each of the 39 characters input array
#     using a prime hash style function. In our C program
#     we'll have two arrays with a universal index, in combination
#     function like a dictionary.

# Script used to quickly test prime values, to experiement in
#     finding a unique key set. Since results are for two 
#     hardcoded arrays in our main.c, they can also be printed
#     for convenience.

import csv
import numpy as np
from collections import Counter

keys = []
mapping = []
allCombos = []

def hash_string(in_str):
    primes = np.array([3,3,5,7,11,13])
    in_set = [int(e) + 1 for e in in_str]
    base = sum([int(e) for e in in_str])
    hashOutput = 1
    primes += base
    for i in range(6):
        hashOutput += primes[in_set[i]] * in_set[i]
    return hashOutput


def isMappingUnique():
  array = []
  for eachPair in mapping:
    array.append(eachPair[1])
  results = dict(Counter(array))
  nonUn = 0

  for key in results:
    if results[key] is not 1:
      nonUn += 1
      print(str(key) + " - " + str(results[key]))

  print("Total Non-Unique keys - "+str(nonUn))
  return (nonUn is 0)

def printKeysForC():
  result = "TYPE? keys[39] = {"
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
        uniqueSum = hash_string(row[2])
        keys.append(uniqueSum)
        mapping.append( (row[0], uniqueSum) )
      lineCount += 1

def hashEachPossibleInput():
  for possibleInputSet in allCombos:
    out_tmp = hash_string(possibleInputSet)
    mapping.append( (possibleInputSet, out_tmp) )




def getAllCombos():
  for a in range(0,3):
    if a is 0:
      allCombos.append("000000")
    else:
      for b in range(0,3):
        if b is 0:
          allCombos.append(str(a) + "00000")
        else:
          for c in range(0,3):
            if c is 0:
              allCombos.append(str(a) + str(b) + "0000")
            else:
              for d in range(0,3):
                if d is 0:
                  allCombos.append(str(a) + str(b) + str(c) + "000")
                else:
                  for e in range(0,3):
                    if e is 0:
                      allCombos.append(str(a) + str(b) + str(c) + str(d) + "00")
                    else:
                      for f in range(0,3):
                        allCombos.append(str(a)+str(b)+str(c)
                        +str(d)+str(e)+str(f))


print("")
getAllCombos()
#print("Values to hash - " + str(len(allCombos)))
hashEachPossibleInput()
# processCSV()
print(keys)
print("Keys are Unique? - " + str(isMappingUnique()))
#printCharsForC()
#printKeysForC()
print(mapping)

