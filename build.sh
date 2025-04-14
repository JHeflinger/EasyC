# total timing
t_startTime=$(date +%s%N)

# initialize vars for building
SRC_DIR="include"
CDIR="$(pwd)"
INCLUDES=""
SOURCES=""
OBJECTS=""
LIBS=""
LINKS=""

# production build flags
PROD=""
if [ "$1" == "prod" ]; then
	echo "Optimizing for production build..."
	PROD="-O3 -DPROD_BUILD"
fi

# create build directory if it does not exist
if [ ! -d "build" ]; then
	mkdir "build"
fi

# create all build directories if it does not exist
cd build
if [ ! -d "cache" ]; then
	mkdir "cache"
fi
cd cache
if [ ! -d "src" ]; then
	mkdir "src"
fi
cd ..
cd ..

# set up cache folders
find "$SRC_DIR" -type d | while read -r SUBPATH; do
	REL="${SUBPATH#$CDIR/$SRC_DIR}"
	DESTDIR="build/cache/$REL"
	if [ ! -d "$DESTDIR" ]; then
		mkdir -p "$DESTDIR"
	fi
done

# get includes
while IFS= read -r dir; do
	INCLUDES="$INCLUDES -I$dir"
done < <(find "$SRC_DIR" -type d)

# compile obj files
echo "Compiling sources..."
startTime=$(date +%s%N)
SOURCES_UP_TO_DATE="true"
FOUND_MAIN="false"
while IFS= read -r file; do
	REL="${file#$CDIR/$SRC_DIR}"
	DESTDIR="build/cache/$REL"
	filename=$(basename "$file")
	if [ "$filename" != "main.c" ]; then
		if [ ! -f $DESTDIR ]; then
			SOURCES_UP_TO_DATE="false"
			echo -e "- [$filename] \033[33m(compiling...)\033[0m"
			gcc -Wall -Wextra -Wno-unused-parameter -c $file$INCLUDES$LIBS$LINKS -o $DESTDIR.o $PROD
			if [ $? -ne 0 ]; then
				echo -e "Building source \"$filename\" \033[31mfailed\033[0m"
				exit 1
			fi
			echo -e "\033[1A\033[0K- [$filename] \033[32mOK\033[0m"
			cp $file $DESTDIR
		else
			if ! cmp -s $file $DESTDIR; then
				SOURCES_UP_TO_DATE="false"
				echo -e "- [$filename] \033[33m(compiling...)\033[0m"
				gcc -Wall -Wextra -Wno-unused-parameter -c $file$INCLUDES$LIBS$LINKS -o $DESTDIR.o $PROD
				if [ $? -ne 0 ]; then
					echo -e "Building source \"$filename\" \033[31mfailed\033[0m"
					exit 1
				fi
				echo -e "\033[1A\033[0K- [$filename] \033[32mOK\033[0m"
				cp $file $DESTDIR
			fi
		fi
		OBJECTS="$OBJECTS $DESTDIR.o"
	else
		FOUND_MAIN="true"
	fi
done < <(find "$SRC_DIR" -type f -name "*.c")
if [ "$SOURCES_UP_TO_DATE" == "true" ]; then
	echo -e "\033[1A\033[0KSources are currently \033[32mup to date\033[0m"
else
	endTime=$(date +%s%N)
	elapsed=$(((endTime - startTime) / 1000000))
	hh=$((elapsed / 3600000))
	mm=$(((elapsed % 3600000) / 60000))
	ss=$(((elapsed % 60000) / 1000))
	cc=$((elapsed % 1000))
	echo -e "\033[32mFinished\033[0m compiling sources in ${hh}:${mm}:${ss}.${cc}"
fi

# compile executable
echo "Building executable..."
startTime=$(date +%s%N)
gcc -Wall -Wextra -Wno-unused-parameter test/main.c$OBJECTS$INCLUDES$LIBS$LINKS -o build/a.out $PROD
if [ $? -ne 0 ]; then
	echo -e "Build \033[31mFailed\033[0m"
	exit 1
fi
endTime=$(date +%s%N)
elapsed=$(((endTime - startTime) / 1000000))
hh=$((elapsed / 3600000))
mm=$(((elapsed % 3600000) / 60000))
ss=$(((elapsed % 60000) / 1000))
cc=$((elapsed % 1000))
echo -e "\033[32mFinished\033[0m building executable in ${hh}:${mm}:${ss}.${cc}"
endTime=$(date +%s%N)
elapsed=$(((endTime - t_startTime) / 1000000))
hh=$((elapsed / 3600000))
mm=$(((elapsed % 3600000) / 60000))
ss=$(((elapsed % 60000) / 1000))
cc=$((elapsed % 1000))
echo -e "\033[32mFinished\033[0m total build in ${hh}:${mm}:${ss}.${cc}"
