import numpy as numpy

import pandas as pd 

CSV_FILE = "/Users/abpwrs/projects/embedded/final/newEncoding.csv"

df = pd.read_csv(CSV_FILE)
print(df['Character'])
print(df['Inputs'])

existing_map = {}
for c, i in zip(df['Character'],df['Inputs']):
    existing_map[c] = str(i)

print(existing_map)


hashed_map = {}
for k, v in existing_map.items():
    hashed_map[k] = int(v,3)
print(hashed_map)

assert len(existing_map.values()) == len(set(hashed_map.values()))
print("assertion passed")


keys = "int keys[39] = {"
chars = "char chars[39] = {"
for k, v in hashed_map.items():
    keys = "{0}, {1}".format(keys, v)
    chars = "{0}, '{1}'".format(chars, k)

keys += '};'
chars += '};'

print(keys)
print(chars)
