CFLAGS = -std=c++17
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
INC_KANJI = -I kanji/
SOURCES = source/main.cpp kanji/kanji.cpp kanji/renderer/renderer.cpp kanji/renderer/buffer.cpp kanji/renderer/window.cpp kanji/renderer/vcontext.cpp kanji/renderer/mesh/vertex.cpp

output: compileshaders
	g++ $(CFLAGS) -o build/build.out $(SOURCES) $(INC_KANJI) $(LDFLAGS)

exec: build/build.out
	./output.sh

compileshaders: kanji/renderer/shaders/main.vert kanji/renderer/shaders/main.frag
	glslc kanji/renderer/shaders/main.vert -o build/shaders/main.vert.spv
	glslc kanji/renderer/shaders/main.frag -o build/shaders/main.frag.spv

	glslc source/mat.vert -o build/shaders/mat.vert.spv
	glslc source/mat.frag -o build/shaders/mat.frag.spv

clean: 
	rm build/build.out