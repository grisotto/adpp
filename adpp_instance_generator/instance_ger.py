"""Instance generator for Ads Rich Placement Problem
Usage:
  instance_adpp.py (DIST) (H) (T) (NADS) 
  instance_adpp.py --version

Examples:
	instance_arpp.py zipf 5 5 100
	instance_arpp.py norm 5 3 1000

Arguments:
  DIST  distributions: norm, zipf
  H     height of the banner/knapsack
  T     exposition time of the banner/knapsack
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
    arguments = docopt(__doc__, version='1.0.0 - 05/02/18')
     
#Validation the distribuition
    schema = Schema({
            'DIST': And(str, Use(str.lower),
                        lambda s: s in ('norm','zipf'),
                        error='Please, use distribuition norm or zipf'),
            'H':    And(Use(int), lambda n: 0 < n < 15), 
            'T':    And(Use(int), lambda n: 0 < n < 15), 
            'NADS':    And(Use(int), lambda n: 0 < n < 10e10), 
            str: object })
try:
    arguments = schema.validate(arguments)
except SchemaError as e:
    exit(e)
# Here we define the numbers of ads and the information
# each ad as height, exposition time(t), how much want to pay to shown

DIST = arguments['DIST']
H = arguments['H'] #banner/knapsack height/lines
T = arguments['T'] #banner/knapsack exposition time
NADS = arguments['NADS']
random.seed(datetime.now())

file = open('{}_{}_{}_{}.adpp'.format(DIST, H, T, NADS),"w")
file.write('{}\t{}'.format(H,T))
file.write('\n{}\n'.format(NADS))
ads = []
for i in range(1, NADS):
    d = {}
    d['h'] = random.randrange(1, H-1,1)
    d['t'] = random.randrange(1, T-1,1)
    area = d['h'] * d['t']
    d['alpha'] = round(random.uniform(0.5*area,1.5*area ),2)
    ads.append(d)
    file.write('{}\t{}\t{}\n'.format(d['h'],d['t'],d['alpha']))


# print(ads)

file.close()

