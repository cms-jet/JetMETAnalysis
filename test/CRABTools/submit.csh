for y in `ls *.cfg`;
do
crab -create -cfg $y;
done
for y in `ls -d */`
do
new_file=`echo $y | sed 's!/!!'`
crab -submit -c ${new_file}
done
