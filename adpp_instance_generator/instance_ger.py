"""Instance generator for Ads Rich Placement Problem
Usage:
  instance_adpp.py (DIST) (H) (T) (NADS) 
  instance_adpp.py --version

Examples:
	instance_arpp.py zipf 5 5 100
	instance_arpp.py norm 5 3 1000

Arguments:
  DIST  distributions: norm, zipf
  H     height of the banner
  T     exposition time of the banner
  NADS  numbers of ads

Options:
  -h --help            show this help message and exit
  --version            show version and exit
"""
import os
import numpy as np
import random 
import math
from datetime import datetime
from docopt import docopt

try:
    from schema import Schema, And, Or, Use, Optional, SchemaError
except ImportError as e:
    exit('This example requires `schema` data-validation library'
            'is installed: \n pip install schema\n')

if __name__ == '__main__':
    arguments = docopt(__doc__, version='1.2.0 - 30/08/18')
     
#Validation the distribuition
    schema = Schema({
            'DIST': And(str, Use(str.lower),
                        lambda s: s in ('norm','zipf'),
                        error='Please, use distribuition norm or zipf'),
            'H':    And(Use(int), lambda n: 0 < n < 1000), 
            'T':    And(Use(int), lambda n: 0 < n < 100000), 
            'NADS':    And(Use(int), lambda n: 0 < n < 10e10), 
            str: object })
try:
    arguments = schema.validate(arguments)
except SchemaError as e:
    exit(e)
# Here we define the numbers of ads and the information
# each ad as height, exposition time(t), how much want to pay to shown

# dimensao do tempo infinita

# definir um k para altura e os itens tem 10 a 20 % de k

DIST = arguments['DIST']
H = arguments['H'] #banner height/lines
# T = arguments['T'] #banner exposition time
T = 100000
NADS = arguments['NADS']
random.seed(datetime.now())

file = open('{}_{}_{}_{}.adpp'.format(DIST, H, T, NADS),"w")
file.write('{}\n'.format(NADS))
file.write('{}\t{}\n'.format(H,T))
ads = []
for i in range(1, NADS):
    d = {}
    hmin = int(H*0.1) 
    hmax = int(H*0.2)
    d['h'] = random.randrange(hmin, hmax, 1)
    d['t'] = random.randrange(1, 10, 1)
    area = d['h'] * d['t']
    d['alpha'] = int (round(random.uniform(1*area,3*area ),2))
    ads.append(d)
    file.write('{}\t{}\t{}\n'.format(d['h'],d['t'],d['alpha']))


# print(ads)

file.close()

