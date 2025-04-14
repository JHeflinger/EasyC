./build.sh
if [ $? -ne 0 ]; then
	exit 1
fi
./build/a.out $1 $2
