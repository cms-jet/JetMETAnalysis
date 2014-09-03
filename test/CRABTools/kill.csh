for y in `ls -d */`
do
new_file=`echo $y | sed 's!/!!'`
crab -kill all -c ${new_file}
done

