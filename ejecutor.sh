i=0
while [ $i -lt 2 ]; do
make clean
make
./simple_bus.x
echo "Running iteration $i"
  i=$((i + 1))
done


