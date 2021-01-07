#$BIN="buffergen"

compile: buffergen.cpp 
	g++ buffergen.cpp -o buffergen -lGL -lGLEW -g	

run:
	./buffergen

debug:
	gdb buffergen	#$(BIN)

clean:
	rm buffergen#$(BIN)


