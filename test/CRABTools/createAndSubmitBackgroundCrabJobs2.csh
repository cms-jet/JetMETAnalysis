for y in `ls *cfg`;
do 
crab -create -cfg $y;
done
for y in `ls -l | egrep '^d'`;
do 
crab -submit -c $y;
done
