#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Jan 17 16:50:03 2021

@author: ricostynha
"""
import numpy as np
import matplotlib.pyplot as plt

def clean_string (string):

    a = string.split('_')
    b = int(a[0])
    c = int(a[1])
    return [b,c]

def get_node_n (string):
    a = string.split('_')
    c = int(a[1])
    return c

def gett_n_messages (string):
        a = string.split(' ')
        b = int(a[-1])
        return b

from os import walk

f = []
results_dic = {}

mypath = './saved_results'

#mypath = './s_t'

test_nnu_prob = []


file_names = []

for (dirpath, dirnames, filenames) in walk(mypath):
    f.extend(filenames)
    test_nnu_prob = dirnames
    break


tests = []

for el in test_nnu_prob:
    tests.append([el,clean_string(el)])
    
results = {}


for test in tests:
    path = mypath + '/' + test[0]
    files_test = []
    for (dirpath, dirnames, filenames) in walk(path):
    
        files_test = filenames;
    num = 0
    r_v = []
    for node in files_test:
        path_file = path + '/' + node
        n  = get_node_n(node)
        f = open(path_file , "r")
        
        first_line = f.readline()
        
       
        for last_line in f:
            pass
        a = gett_n_messages(last_line)
        num += a
        r_v.append(a)
        
    #res = num/test[1][0]
    res = max(r_v)
    results[test[0]] = res
    
 
v = '''

for test in tests:
    path = mypath + '/' + test[0]

    dirs_test = []
    for (dirpath, dirnames, filenames) in walk(path):
        print(dirnames)
        dirs_test = dirnames.copy();
        break
    

    r_sv = []
    for diri in dirs_test:
        path_d = path + "/" + diri

        files_test = []
        for (dirpath, dirnames, filenames) in walk(path_d):
    
            files_test = filenames;
        
        
        num = 0
        r_v = []
        for node in files_test:
            path_file = path_d + '/' + node
            n  = get_node_n(node)
            f = open(path_file , "r")
            
            first_line = f.readline()
            
           
            for last_line in f:
                pass
            a = gett_n_messages(last_line)
            num += a
            r_v.append(a)
            
        #res = num/test[1][0]
        res = max(r_v)
        print(res)
        r_sv.append(res)
    res = np.array(r_sv).mean()
         
    results[test[0]] = res
    

'''
res_list = [(k,v) for k,v in results.items()]




#first dimension number nodes #second measurement
r_node = np.zeros((11,9))

print(r_node)

for node in range(3,11):
    for r in res_list:
        [n,p] = clean_string (r[0])
        if(n == node):
            print(p, r[1])
            r_node[node][int(p/5)] = r[1]
       
        
p_node = r_node.transpose()


 
nodes_x = np.array(range(0,11))
prob_x  = np.array(range(0,9))*5
 
plt.title('Number Messages Vs Error Probability')
plt.xlabel('Error probability (%)')
plt.ylabel('Number of Messages on Leader')

plt.plot(prob_x[:7],r_node[3][:7],label = '3 nodes')
plt.plot(prob_x[:7],r_node[4][:7],label = '4 nodes')
plt.plot(prob_x[:7],r_node[5][:7],label = '5 nodes')


plt.plot(prob_x[:7],r_node[6][:7],label = '6 nodes')
plt.plot(prob_x[:7],r_node[7][:7],label = '7 nodes')
plt.plot(prob_x[:7],r_node[8][:7],label = '8 nodes')
#plt.plot(prob_x,r_node[6],label = '7 nodes')

plt.legend(loc="upper left")

plt.savefig('Num_messages_vs_error_prob.png')
 
plt.show()

nodes_x = np.array(range(0,11))
prob_x  = np.array(range(0,9))*0.5
 
plt.title('Number Messages Vs Node number')
plt.xlabel('Node number')
plt.ylabel('Number of Messages on Leader')

plt.xlim([3, 9])

plt.plot(nodes_x[3:-1],p_node[0][3:-1],label =  'error prob 0%')
plt.plot(nodes_x[3:-1],p_node[1][3:-1],label =  'error prob 5%')
plt.plot(nodes_x[3:-1],p_node[2][3:-1],label =  'error prob 10%')
plt.plot(nodes_x[3:-1],p_node[3][3:-1],label =  'error prob 15%')

plt.plot(nodes_x[3:-1],p_node[4][3:-1],label =  'error prob 20%')
plt.plot(nodes_x[3:-1],p_node[5][3:-1],label =  'error prob 25%')







plt.legend(loc="upper left")


plt.savefig('Num_messages_vs_n_nodespng')
