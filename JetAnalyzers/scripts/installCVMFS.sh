#!/bin/bash

printf “UPDATING PACKAGES\n”
sudo apt-get update
printf “\n\nINSTALLING DEPENDENCIES\n”
sudo apt-get install -qq autofs attr gdb git uuid-dev uuid fuse
printf “\n\nADDING CVM-FS RELEASE LOCATION TO APT-GET\n”
wget https://ecsft.cern.ch/dist/cvmfs/cvmfs-release/cvmfs-release-latest_all.deb
sudo dpkg -i cvmfs-release-latest_all.deb
rm -f cvmfs-release-latest_all.deb
printf “\n\nUPDATING PACKAGES\n”
sudo apt-get update
printf “\n\nINSTALLING CVM-FS\n”
sudo apt-get install -qq cvmfs cvmfs-config-default
printf “\n\nCONFIGURING CVM-FS\n”
sudo cvmfs_config setup
printf “\n\nSETTING UP /etc/cvmfs/default.local\n”
#Options: DIRECT, http://ca-proxy.cern.ch:3128;http://ca-proxy1.cern.ch:3128|http://ca-proxy2.cern.ch:3128|http://ca-proxy3.cern.ch:3128|http://ca-proxy4.cern.ch:3128|http://ca-proxy5.cern.ch:3128
sudo echo CVMFS_HTTP_PROXY=\"DIRECT\" | sudo tee /etc/cvmfs/default.local > /dev/null
#Options: cms.cern.ch, grid.cern.ch, atlas.cern.ch, lhcb.cern.ch, ...
sudo echo CVMFS_REPOSITORIES=cms.cern.ch | sudo tee -a /etc/cvmfs/default.local > /dev/null
printf “\n\nRESTART AUTOFS\n”
sudo service autofs restart
printf “\n\nINITIATING CVM-FS PROBE\n”
cvmfs_config probe

# To check the setup
# ==================
# cvmfs_config chksetup

# To show the configuration
# =========================
# cvmfs_config showconfig cms.cern.ch

# To manually mount /cvmfs/cms.cern.ch/
# =====================================
# 	mkdir -p /mnt/cvmfs
# 	mount -t cvmfs cms.cern.ch /mnt/cvmfs

# Resources/Links:
# ================
# ATLAS CVMFS Intall Manual: https://twiki.cern.ch/twiki/bin/view/AtlasComputing/Cvmfs21
# LHCB YAML File Example: https://github.com/ibab/travis-lhcb/blob/master/.travis.yml
# Travis CI: https://devsector.wordpress.com/2014/11/09/github-continuous-integration-with-travis-ci/
# C++ Unit Test Frameworks: http://accu.org/index.php/journals/1326