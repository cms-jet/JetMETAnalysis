for y in `ls -d */`
do
new_file=`echo $y | sed 's!/!!'`
crab -getoutput -c ${new_file}
done

