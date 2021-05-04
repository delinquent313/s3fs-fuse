echo "CSC4420 RC4 Standalone Test"

FILE=testfile
if test -f "$FILE"; then
    echo "$FILE exists."
else
    echo "$FILE please create $FILE or rename existing file to $FILE"
    rm testn* tests*
    exit 0
fi
echo "commands executed test openssl nosalt, openssl salt, encryption and decryption "
echo "  rm testn* tests*"
echo "  ls -l"
echo "  ./rc4 testfile testnosalt.rc4 'csc4420isveryfun' -en -nosalt "
echo "  openssl rc4 -e -k csc4420isveryfun -in testfile -out testnosalt.openssl -nosalt "
echo "  diff testnosalt.openssl testnosalt.rc4 "
echo "  ./rc4 testnosalt.rc4 testnosalt.decrypted 'csc4420isveryfun' -de -nosalt"
echo "   diff testfile testnosalt.decrypted"

echo "  ./rc4 testfile testsalt.rc4 'csc4420isveryfun' -en "
echo "  openssl rc4 -e -k csc4420isveryfun -in testfile -out testsalt.openssl"
echo "  ./rc4 testsalt.rc4 testsalt.decrypted 'csc4420isveryfun' -de "
echo "  diff testfile testsalt.decrypted"
echo "  ls -l"
rm testn* tests*
ls -l
./rc4 testfile testnosalt.rc4 'csc4420isveryfun' -en -nosalt 
sleep 1
openssl rc4 -e -k csc4420isveryfun -in testfile -out testnosalt.openssl -nosalt 
sleep 1
diff testnosalt.openssl testnosalt.rc4 
sleep 1
./rc4 testnosalt.rc4 testnosalt.decrypted 'csc4420isveryfun' -de -nosalt
diff testfile testnosalt.decrypted
sleep 2

./rc4 testfile testsalt.rc4 'csc4420isveryfun' -en 
sleep 1
openssl rc4 -e -k csc4420isveryfun -in testfile -out testsalt.openssl
sleep 1
./rc4 testsalt.rc4 testsalt.decrypted 'csc4420isveryfun' -de 
sleep 1
diff testfile testsalt.decrypted
sleep 2
ls -l
echo "diff should not display diffrence in the compared files"
echo "try increasing the file size using fallocate -l {size} testfile and running again or use a bigger testfile"
