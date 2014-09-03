#! /usr/bin/env python
import os

SAMPLE =    [#"QCDFlat356Herwig",
             "JRA"]


path = "/pnfs/cms/WAX/resilient/aperloff/JEC/JRA_outfiles_smaller"
local = "/uscms_data/d2/aperloff/newJRA/"
for ss in SAMPLE:
  if (os.path.exists(local+ss)):
    print "directory "+local+ss+" exists"
  else:
    os.mkdir(local+ss)
  FILES = os.listdir(path)
  for ll in FILES:
    #print ll
    if (ll.find(ss) != -1):
      print ll
      command = "dccp "+path+"/"+ll+" "+local+ss+"/"
      print command
      os.system(command)

  command = "cd "+local+ss+"/"
  os.system(command)
  command = "hadd -f "+local+ss+"/"+ss+".root "+local+ss+"/"+ss+"_*.root"
  os.system(command)
  command = "rm "+local+ss+"/"+ss+"_*.root"
  os.system(command)
                                                                    

