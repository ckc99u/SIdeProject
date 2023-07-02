import numpy as np
import cv2 as cv
import matplotlib.pyplot as plt
from PIL import Image
import matplotlib.image as reader
import math
import sys
sys.setrecursionlimit(10000)
"""
Octree recursive testing
"""

def first_node(tx0, ty0, tz0, txm, tym, tzm, answer):
    maximum = max(max(tx0, ty0), tz0)
    #answer = 0
    if(maximum == tx0):
        if(tym < tx0):
            answer|= 2
        if(tzm < tx0):
            answer|=1
    elif(maximum == ty0):# PLANE [2] 1357
        if (txm < ty0):
            answer |= 4 # set bit at position 0
        if (tzm < ty0):
            answer |= 1 # set bit at position 1
    elif(maximum == tz0):# PLANE [1] 2367
        if (txm < tz0):
            answer |= 4 # set bit at position 2
        if (tym < tz0):
            answer |= 2 # set bit at position 0	

    return answer

def OctreeProcess(octree, rayO, rayD, box0, box1):
    curV = 0
    stack = np.array([.1,.0,.0,.0])
    IDX = np.array([0,0])
    kid = 0
    a= 0
    rayD = 1/rayD

    if(rayD[2] < 0):
        a |= 1
        rayD[2] = abs(rayD[2])
    if(rayD[0] < 0):
        a|= 4
        rayD[0] = abs(rayD[0])
    if(rayD[1] < 0):
        a |= 2
        rayD[1] = abs(rayD[1])
    
    fplane = (box0 - rayO) * rayD
    splane = (box1 - rayO) * rayD
    c = np.greater(splane , fplane)
    closest = splane
    furthest = fplane
    
    if(c.all()):
        closest = fplane
        furthest = splane
    
    tentry = max(closest[0], max(closest[1], closest[2]))
    texit = min(furthest[0], min(furthest[1], furthest[2]))##smaller than 0
    if (texit <= tentry):
        return IDX
    curNode = 0
    pos = a
    pre1 = furthest
    pre2 = closest
    for depth in range(0,6):
        tmx = 0.5 * (furthest[0] + closest[0])
        tmy = 0.5 * (furthest[1] + closest[1])
        tmz = 0.5 * (furthest[2] + closest[2])
        if (furthest[0] < 0 or furthest[1] < 0 or furthest[2] < 0):
            print("exit < 0 ", depth)
            
            if(stack.all() == 0):
                curNode = kid
                closest = pre2
                furthest = pre1
                tmx = 0.5 * (furthest[0] + closest[0])
                tmy = 0.5 * (furthest[1] + closest[1])
                tmz = 0.5 * (furthest[2] + closest[2])
                pos = a^kid
                #break
            else:
                curNode = kid
                closest = pre2
                furthest = pre1
                tmx = 0.5 * (furthest[0] + closest[0])
                tmy = 0.5 * (furthest[1] + closest[1])
                tmz = 0.5 * (furthest[2] + closest[2])
                pos = a^kid
                
            
        else:
            curNode = first_node(closest[0], closest[1], closest[2], tmx, tmy, tmz, 0)

        if pos >= 8: break

        if(octree[curV,pos*3+2][0] == 0 and octree[curV,pos*3+2][1] == 0 ):
            print("leafnode(curV, kid): ", curV, pos)
            print("skip slice: ", octree[curV,pos*3+1][0])
            if(octree[curV,pos*3+2][2]==0):
                IDX[0] = -1
            else: IDX[0] =1
            IDX[1] = octree[curV,pos*3+1][0]
            depth -= 1
            curNode = kid
            return IDX

        else:
            print("parent node(curV, kid)", curV, pos)

        pre1 = furthest
        pre2 = closest

        if (curNode == 0):
            furthest = [tmx, tmy, tmz]
            curV = octree[curV, pos*3+2][0] + octree[curV, pos*3+2][1]*255 
            kid = new_node(tmx, 4, tmy, 1, tmz, 2)##put in stack
        elif(curNode == 1):
            closest[1] = tmy
            furthest = [tmx, furthest[1], tmz]
            curV = octree[curV, pos*3+2][0] + octree[curV, pos*3+2][1]*255 
            pos = a^1
            kid = new_node(tmx, 5, furthest[1], 8, tmz, 3)
        elif(curNode == 2):
            curV = octree[curV, pos*3+2][0] + octree[curV, pos*3+2][1]*255 
            pos = a^2
            closest = [closest[0], closest[1], tmz]
            furthest = [tmx, tmy, furthest[2]]
            kid = new_node(tmx, 6, tmy, 3, furthest[2], 8)
        elif(curNode == 3):

            closest = [closest[0], tmy, tmz]
            furthest = [tmx, furthest[1], furthest[2]]
            curV = octree[curV, pos*3+2][0] + octree[curV, pos*3+2][1]*255 
            pos = a^3
            kid = new_node(tmx, 7, furthest[1], 8, furthest[2], 8)
        elif(curNode == 4):

            closest = (tmx, closest[1], closest[2])
            furthest = (furthest[0], tmy, tmz)
            curV = octree[curV, pos*3+2][0] + octree[curV, pos*3+2][1]*255 
            pos = a^4
            kid = new_node(furthest[0], 8, tmy, 5, tmz, 6)
        elif(curNode == 5):
            
            closest = (tmx, tmy, closest[2])
            furthest = (furthest[0], furthest[1], tmz)
            curV = octree[curV, pos*3+2][0] + octree[curV, pos*3+2][1]*255 
            pos = a^5
            kid = new_node(tmx, 8, furthest[1], 8, furthest[2], 7)
        elif(curNode == 6):
            
            closest = (tmx, closest[1], tmz)
            furthest = (furthest[0], tmy, furthest[2])
            curV = octree[curV, pos*3+2][0] + octree[curV, pos*3+2][1]*255
            pos = a^6
            kid = new_node(furthest[0], 8, tmy, 7, furthest[2], 8)
        elif(curNode == 7):
            closest = (tmx, tmy, tmz)
            curV = octree[curV, pos*3+2][0] + octree[curV, pos*3+2][1]*255
            pos = a^7
            kid = 8
        else:
            break
        
    return IDX


def new_node(txm, x, tym, y, tzm, z):
    if(txm < tym):
        if(txm < tzm ): return x
    else:
        if(tym < tzm): return y
    return z
def recus(octree,tx0, ty0, tz0, tx1, ty1, tz1, kid, node):
    txm = tym = tzm = 0.0
    curNode = 0
    if(tx1 < 0 or ty1 < 0 or tz1 < 0 ):
        print("exit < 0 ", kid)
        return
    if(node >= octree.shape[0]):
        print("exceed tree shape")
        return
        
    if(octree[node,kid*3+2][0] == 0 and octree[node,kid*3+2][1] == 0):
        print("leafnode(curV, kid): ", node, kid)
        if(octree[node,kid*3+2][2] == 0):
            print("skip slice: ", octree[node,kid*3+1][0])
        else:
            print("marching slice: ", octree[node,kid*3+1][0])
        return
    else:
        print("parent node(curV, kid)", node, kid)
    
    txm = .5*(tx0+tx1)
    tym = .5*(ty0+ty1)
    tzm = .5*(tz0+tz1)
    curNode = first_node(tx0, ty0, tz0, txm, tym, tzm, 0)
    while True:
        #print(curNode)
        if (curNode == 0):
            curV = octree[node,kid*3+2][0] + octree[node,kid*3+2][1]*255 
            recus(octree, tx0, ty0, tz0, txm, tym ,tzm, kid, curV)
            curNode = new_node(txm, 4, tym, 1, tzm, 2)
        elif (curNode == 1):
            curV = octree[node,kid*3+2][0] + octree[node,kid*3+2][1]*255 
            recus(octree, tx0, tym, tz0, txm, ty1 ,tzm, kid^1, curV)
            curNode = new_node(txm, 5, ty1, 8, tzm, 3)
        elif(curNode == 2):
            curV = octree[node,kid*3+2][0] + octree[node,kid*3+2][1]*255 
            recus(octree, tx0, ty0, tzm, txm, tym ,tz1, kid^2, curV)
            curNode = new_node(txm, 6, tym, 3, tz1, 8)
        elif(curNode == 3):
            curV = octree[node,kid*3+2][0] + octree[node,kid*3+2][1]*255 
            recus(octree, tx0, tym, tzm, txm, ty1 ,tz1, kid^3, curV)
            curNode = new_node(txm, 7, ty1, 8, tz1, 8)
        elif(curNode == 4):
            curV = octree[node,kid*3+2][0] + octree[node,kid*3+2][1]*255 
            recus(octree, txm, ty0, tz0, tx1, tym, tzm, kid^4, curV)
            curNode = new_node(tx1, 8, tym, 5, tzm, 6)
        elif(curNode == 5):
            curV = octree[node,kid*3+2][0] + octree[node,kid*3+2][1]*255 
            recus(octree, txm, tym, tz0, tx1, ty1, tzm, kid^5, curV)
            curNode = new_node(tx1, 8, ty1, 8, tzm, 7)
        elif(curNode == 6):
            curV = octree[node,kid*3+2][0] + octree[node,kid*3+2][1]*255 
            recus(octree, txm, ty0, tzm, tx1, tym, tz1, kid^6, curV)
            curNode = new_node(tx1, 8, tym, 7, tz1, 8)
        elif(curNode == 7):
            curV = octree[node,kid*3+2][0] + octree[node,kid*3+2][1]*255 
            recus(octree, txm, tym, tzm, tx1, ty1, tz1, kid^7, curV)
            curNode = 8
        else:
            break


    
def recursiv_tree(octree, rayO, rayD, box0, box1):
    kid = 0
    #node = np.array([0,0,0])
    rayD = 1/rayD
    if(rayD[2] < 0):
        kid |= 1
        rayD[2] = abs(rayD[2])
    if(rayD[0] < 0):
        kid|= 4
        rayD[0] = abs(rayD[0])
    if(rayD[1] < 0):
        kid |= 2
        rayD[1] = abs(rayD[1])
    
    fplane = (box0 - rayO) * rayD
    splane = (box1 - rayO) * rayD
    c = np.greater(splane , fplane)
    closest = splane
    furthest = fplane
    if(c.all()):
        closest = fplane
        furthest = splane
    
    tentry = max(closest[0], max(closest[1], closest[2]))
    texit = min(furthest[0], min(furthest[1], furthest[2]))##smaller than 0
    if(texit > tentry):
        recus(octree, closest[0], closest[1], closest[2], furthest[0], furthest[1], furthest[2], kid, 0)#0 root
    

img = "cleaned100016C1/treemap.bmp"
ray_dir = np.array([-.2, .2, .2])
v = np.linalg.norm(ray_dir)
ray_dir = v/ray_dir

rayO = np.array([0.8, 0.8, 0.01])#np.array([-2.0,-2.0,-2.0])np.array([0.1, 0.8, 0.01])
box_min = [0,0,0.11]
box_max = [1,1,0.89]
octree = reader.imread(img)

print("======recursive answer: \n")
recursiv_tree(octree,rayO , ray_dir, box_min, box_max)

print("===for loop answer")
for i in range(3):
    print("round: ", i)
    Answer = OctreeProcess(octree,rayO , ray_dir, box_min, box_max)
    #print("skip space y/n: ", Answer)
    
    if(Answer[0] != 0):
        #step = ray_dir * Answer[1]
        #print(step)
        rayO += Answer[1]*ray_dir*.00293
        print(rayO)
    else:
        step = ray_dir *Answer[1]*.00293
        rayO += step
        print(rayO)


