#
# locate all the header dependencies:
for file in ../../../boost/regex/*.hpp ; do
	if [ -f $file ]; then
		header="$header $file"
	fi
done

for file in ../../../boost/regex/v3/*.hpp; do
	if [ -f $file ]; then
		header="$header $file"
	fi
done

for file in ../../../boost/regex/v3/*.hxx; do
	if [ -f $file ]; then
		header="$header $file"
	fi
done

#
# locate all the source files:
for file in ../src/*.cpp; do
	if [ -f $file ]; then
		src="$src $file"
	fi
done


