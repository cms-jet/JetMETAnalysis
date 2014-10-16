for y in `ls -d */`
do
new_file=`echo $y | sed 's!/!!'`
crab -status -c ${new_file}
done

