#!/usr/bin/python

import cv2
import numpy as np
import json
import sys
from matplotlib import pyplot as plt

img = cv2.imread(sys.argv[1])
gray= cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
sift = cv2.xfeatures2d.SIFT_create()
kp, des = sift.detectAndCompute(gray,None)

print(kp, des)
np.save(sys.argv[1].split(".")[0]+".npy", des)

img1 = cv2.imread(sys.argv[1])
gray1= cv2.cvtColor(img1,cv2.COLOR_BGR2GRAY)
img2 = cv2.imread('./46.jpg')
gray2= cv2.cvtColor(img2,cv2.COLOR_BGR2GRAY)

orb = cv2.xfeatures2d.SIFT_create()

# find the keypoints and descriptors with SIFT
kp1, des1 = orb.detectAndCompute(gray1,None)
kp2, des2 = orb.detectAndCompute(gray2,None)

# create BFMatcher object
bf = cv2.BFMatcher()

# Match descriptors.
matches = bf.knnMatch(des1,des2,k=2)

good = []
for m,n in matches:
    if m.distance < 0.75*n.distance:
        good.append([m])

# Draw first 10 matches.
img3 = cv2.drawMatchesKnn(img1,kp1,img2,kp2,good, None, flags=2)

plt.imshow(img3),plt.show()
