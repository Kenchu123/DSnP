# echo "=== Testing do1 ==="
# ../adtTest.dlist -f do1 > out.mine
# ../ref/adtTest.dlist -f do1 > out.ref
# colordiff out.mine out.ref
echo "=== Testing do2 ==="
../adtTest.dlist -f do2 > out.mine
../ref/adtTest.dlist -f do2 > out.ref
colordiff out.mine out.ref
# echo "=== Testing do3 ==="
# ../adtTest.dlist -f do3 > out.mine
# ../ref/adtTest.dlist -f do3 > out.ref
# colordiff out.mine out.ref
# echo "=== Testing do4 ==="
# ../adtTest.dlist -f do4 > out.mine
# ../ref/adtTest.dlist -f do4 > out.ref
# colordiff out.mine out.ref
# echo "=== Testing do5 ==="
# ../adtTest.dlist -f do5 > out.mine
# ../ref/adtTest.dlist -f do5 > out.ref
# colordiff out.mine out.ref

# $1 -f do2
# $1 -f do3
# $1 -f do4
# $1 -f do5
