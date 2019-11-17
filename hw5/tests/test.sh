echo "=== Testing do1 ==="
../adtTest.bst -f do1 > out.mine
../ref/adtTest.bst -f do1 > out.ref
colordiff out.mine out.ref
# echo "=== Testing do2 ==="
# ../adtTest.array -f do2 > out.mine
# ../ref/adtTest.array -f do2 > out.ref
# colordiff out.mine out.ref
# echo "=== Testing do3 ==="
# ../adtTest.array -f do3 > out.mine
# ../ref/adtTest.array -f do3 > out.ref
# colordiff out.mine out.ref
# echo "=== Testing do4 ==="
# ../adtTest.array -f do4 > out.mine
# ../ref/adtTest.array -f do4 > out.ref
# colordiff out.mine out.ref
# echo "=== Testing do5 ==="
# ../adtTest.array -f do5 > out.mine
# ../ref/adtTest.array -f do5 > out.ref
# colordiff out.mine out.ref

# $1 -f do2
# $1 -f do3
# $1 -f do4
# $1 -f do5
