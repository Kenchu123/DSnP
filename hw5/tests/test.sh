# echo "=== Testing do1 ==="
# ../adtTest.bst -f do1 > out.mine
# ../ref/adtTest.bst -f do1 > out.ref
# colordiff out.mine out.ref
# echo "=== Testing do2 ==="
# ../adtTest.array -f do2 > out.mine
# ../ref/adtTest.array -f do2 > out.ref
# colordiff out.mine out.ref
# echo "=== Testing do3 ==="
# ../adtTest.bst -f do3 > out.mine
# ../ref/adtTest.bst -f do3 > out.ref
# colordiff out.mine out.ref
# echo "=== Testing do4 ==="
# ../adtTest.bst -f do4 > out.mine
# ../ref/adtTest.bst -f do4 > out.ref
# colordiff out.mine out.ref
# echo "=== Testing do5 ==="
# ../adtTest.bst -f do5 > out.mine
# ../ref/adtTest.bst -f do5 > out.ref
# colordiff out.mine out.ref

echo "=== Testing mydo ==="

echo "== Testing array ref ==="
../ref/adtTest.array -f mydo > array_out.ref
echo "== Testing array mine ==="
../adtTest.array -f mydo > array_out.mine
colordiff array_out.mine array_out.ref > array_diff

echo "== Testing dlist ref ==="
../ref/adtTest.dlist -f mydo > dlist_out.ref
echo "== Testing dlist mine ==="
../adtTest.dlist -f mydo > dlist_out.mine
colordiff dlist_out.mine dlist_out.ref > dlist_diff

echo "== Testing bst ref ==="
../ref/adtTest.bst -f mydo > bst_out.ref
echo "== Testing bst mine ==="
../adtTest.bst -f mydo > bst_out.mine
colordiff bst_out.mine bst_out.ref > bst_diff

# $1 -f do2
# $1 -f do3
# $1 -f do4
# $1 -f do5
