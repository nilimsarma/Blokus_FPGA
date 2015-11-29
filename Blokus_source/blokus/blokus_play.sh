rm log.txt
make clean
make
for i in $(seq 20 -1 9)
do
for j in $(seq 20 -1 9)
do
echo $i/$j: >> log.txt
./blokus-host -bt -x $i -y $j >> log.txt
done
done
