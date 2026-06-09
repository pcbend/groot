#!/usr/bin/env python3

#https://www.nndc.bnl.gov/ensdfarchivals/distributions/files.json


import requests
import json
import zipfile
import os
import time

from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
ROOT_DIR   = SCRIPT_DIR.parent

BUILD_DIR  = ROOT_DIR / "build"
DATA_DIR   = BUILD_DIR / "data"

ENSDF_DIR  = DATA_DIR / "ensdf"
XUNDL_DIR  = DATA_DIR / "xundl"


try:
    from tqdm import tqdm
    USE_TQDM = True
except ImportError:
    USE_TQDM = False

#https://www.nndc.bnl.gov/xundlarchivals/distributions/dist22/xundl_220329_all.zip


def get_ensdf(): 
  base='https://www.nndc.bnl.gov/ensdfarchivals/'
  url = base + "distributions/files.json"
  page = requests.get(url).text
  data = json.loads(page)
  year   = data['latest']['year']
  latest = data['latest']['files'][0]
  full_path = base+'distributions/dist'+year[-2:]+'/'+latest

  response    = requests.get(full_path,stream=True)
  if response.status_code == 200:
    fileSize    = int(response.headers.get('content-length', 0))
    if USE_TQDM:
      progressBar = tqdm(total=fileSize,unit='iB',unit_scale=True)
    else:
      progressBar = None
    with open(latest,'wb') as f:
      for data in response.iter_content(1024):
        if progressBar:
          progressBar.update(len(data))
        f.write(data)   
    if progressBar:
      progressBar.close()
    
    if not (os.path.exists(ENSDF_DIR)):
      os.makedirs(ENSDF_DIR)
    
    with zipfile.ZipFile(latest,'r') as zip_ref:
      zip_ref.extractall(ENSDF_DIR)
    
    return latest



def get_xundl(): 

  base='https://www.nndc.bnl.gov/xundlarchivals/'
  url = base + "distributions/files.json"
  page = requests.get(url).text
  data = json.loads(page)
  year   = data['latest']['year']
  latest = data['latest']['files'][0]
  full_path = base+'distributions/dist'+year[-2:]+'/'+latest

  response    = requests.get(full_path,stream=True)
  if response.status_code == 200:
    fileSize    = int(response.headers.get('content-length', 0))
    progressBar = tqdm(total=fileSize,unit='iB',unit_scale=True)
    with open(latest,'wb') as f:
      for data in response.iter_content(1024):
        progressBar.update(len(data))
        f.write(data)   
    progressBar.close()
    
    if not (os.path.exists(XUNDL_DIR)):
      os.makedirs(XUNDL_DIR)
    
    with zipfile.ZipFile(latest,'r') as zip_ref:
      zip_ref.extractall(XUNDL_DIR)
    
    return latest

def get_latest():
  #print('downloading and extracting ensdf...    ',end="",flush=True)
  print('downloading and extracting ensdf...    ')
  startTime = time.time()
  ensdfzip = get_ensdf()
  stopTime = time.time();
  print("done in " + str(stopTime-startTime) + "  seconds.",flush=True)
  #print('downloading and extracting xundl...    ',end="",flush=True)
  print('downloading and extracting xundl...    ')
  startTime = time.time()
  xundlzip = get_xundl()
  stopTime = time.time();
  print("done in " + str(stopTime-startTime) + "  seconds.",flush=True)
  print("cleaning up...")
  print("\t removing " + ensdfzip)
  os.remove(ensdfzip)
  print("\t removing " + xundlzip)
  os.remove(xundlzip)

if __name__=="__main__":
  get_latest()
  

