CFLAGS = -std=c++17
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
INC_KANJI = -I kanji/
SOURCES = source/main.cpp kanji/kanji.cpp kanji/renderer/vapp.cpp kanji/renderer/vertex.cpp

output:
	g++ $(CFLAGS) -o build/output $(SOURCES) $(INC_KANJI) $(LDFLAGS)

exec: build/output
	./output.sh

compileshaders: kanji/renderer/shaders/main.vert kanji/renderer/shaders/main.frag
	glslc kanji/renderer/shaders/main.vert -o build/shaders/main.vert.spv
	glslc kanji/renderer/shaders/main.frag -o build/shaders/main.frag.spv

clean: 
	rm build/output