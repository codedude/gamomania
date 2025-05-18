asan:
	LSAN_OPTIONS=suppressions=asan.suppr ./build/Debug/gamomania &> asan.out

debug:
	./build/Debug/gamomania

stats:
	cloc src
