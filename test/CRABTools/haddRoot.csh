for y in `ls -d */`
do
new_file=`echo $y | sed 's!/!!'`
hadd ${new_file}.root ${new_file}/res/*root
done
