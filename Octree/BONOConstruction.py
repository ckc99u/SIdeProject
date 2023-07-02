
import numpy as np
from  matplotlib import pyplot as plt
import matplotlib.image as reader
import math
from glob import glob
import os
from PIL import Image

def Calculate_density(vmin, vmax, alpha, k, Z, slicethick, nums, tests):
    avg_density = 0
    slice_s = math.floor((vmin[2]- Z)/slicethick); slice_f = math.floor((vmax[2]- Z)/slicethick)##change it
    
    if(k ==0 or k==2):
        row_s = math.floor(512-vmax[0]); colume_s = math.floor(512 - vmax[1])
        row_f = math.floor(512-vmin[0]); colume_f = math.floor(512 - vmin[1])
    
    elif(k ==1 or k==3):
        row_s = math.floor(vmin[0]); colume_s = math.floor(vmin[1])
        row_f = math.floor(vmax[0]); colume_f = math.floor(vmax[1])
    elif(k ==4 or k==6):
        row_s = math.floor(vmin[0]); colume_s = math.floor(vmin[1])
        row_f = math.floor(vmax[0]); colume_f = math.floor(vmax[1])
    else:#5,7
        row_s = math.floor(512-vmax[0]); colume_s = math.floor(512 - vmax[1])
        row_f = math.floor(512-vmin[0]); colume_f = math.floor(512 - vmin[1])
    
    total = (slice_f - slice_s)*abs(vmax[1] - vmin[1])*abs(vmax[0] - vmin[0])##total space: number of slices * x pixels * y pixels
    
    for depth in range(slice_s, slice_f):
        if(depth < nums):##debug only
            extract = alpha[depth]##current slice
            for r in range(row_s, row_f):
                rows = extract[r]
                avg_density += sum(rows[colume_s:colume_f])
      
                
    avg_density = round(avg_density/total)
    
    #debug: check image empty position
    if(avg_density ==0):
        
        for depth in range(slice_s, slice_f):
            extract = tests[depth]
            #print(depth,
            #row_s, row_f,  colume_s, colume_f,k)
            extract[row_s: row_f, colume_s: colume_f] = 250
            #plt.imshow(extract)
            #plt.show()
            #break
    
    s = slice_f - slice_s
    if(s == 0):
        s=1
    return avg_density, s##return avg and skipped slices
def InitOctree(alpha, path, slicethickness, Z, max_r, tests):

    child = True
    ##box local shape unit 1x1x0.5
    Octree = np.zeros([1,25,3], dtype=np.float32)##[rgb,rgb,rgb] =[min,max,(ptr,density)]
    pos_rows = 0; pos_colume = 0
    minZ = round((512 - Z)*.5)
    maxZ = 512 - minZ
    nums = Z/slicethickness
    ###store parent pos in first kid
    Octree[0][0] = [0.0, 0., minZ]#min change it
    Octree[0][1] =  [512., 512., maxZ]#max change it
    print("minZ, maxZ", minZ, maxZ)
    for phase in range(6):
        if(phase == 0):##xz front
            
        #elif(phase == 1):#xz back

    
    

def CreateArray(alpha, path, ID, slicethickness, Z, test):
    rows_data_alpha = []
    tests = []
    all_ = 0
    for i in range(len(alpha)):
        img = reader.imread(alpha[i])
        img2 = reader.imread(test[i])
        ##shape to one channel
        #dim1_img = np.dot(img[...,:3],[1,0,0])
        img = np.where(img <= 7, 0, img)##remove background
        img2 = np.where(img2 <= 7, 0, img2)
        k = 0
        #if(i == 0):
        
        for r in range(512):
            c = img[r]
            k += sum(c[0:512])
        all_ += (k/pow(512,2))
        tests.append(img2)
        #plt.imshow(img)
        #plt.show()
        rows_data_alpha.append(img)
    path += ("/"+ ID )

    all_ /= (Z/slicethickness)
    InitOctree(rows_data_alpha, path, slicethickness, Z,all_, tests)
def CreateImage(file_used, path):
    """
    transform rows data to ue4 format
    """
    
    
    Octreef = np.load(file_used).astype(np.float32) 
    tree_shape = Octreef.shape[0]
    
    
    for rows in range(tree_shape):
        for tag in range(0,24,3):
            Octreef[rows][tag] = np.round(255*Octreef[rows][tag])
            Octreef[rows][tag+1] = np.round(255*Octreef[rows][tag+1])
            

    Octreei = np.array(Octreef).astype(np.uint8)

    if(tree_shape <= 1000):
        bmp=Image.fromarray(Octreei)
        bmp.save(path + "/treemap_opt.bmp")
    else:
        print("tree size too long, not suitable, adjust max thresold density to shrink tree size. ")


    




def executeOctree(path, ID, slicethickness):
    print("Creating Octree...")
    img_datas1= sorted(glob(path + "/AlphaImgges/*.jpg"), key=os.path.getmtime)
    img_datas2= sorted(glob(path + "/AlphaImgges/*.jpg"), key=os.path.getmtime)
    files = glob(path+"/AlphaImgges/empty?.jpg")
    numberofslices = len(img_datas1) - len(files)
    print("Size of volume:", numberofslices)
    Z = (numberofslices)*slicethickness
    CreateArray(img_datas1, path, ID, slicethickness, Z, img_datas2)
    tree_npy = path +"/"+ ID + "octree.npy"
    CreateImage(tree_npy, path)
    

