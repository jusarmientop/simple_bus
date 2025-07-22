i=0
while [ $i -lt 100 ]; do
make clean
make
./simple_bus.x
echo "Running iteration $i"
  i=$((i + 1))
done


