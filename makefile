CFLAGS = -std=c++17
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

output: source/main.cpp
	g++ $(CFLAGS) -o build/output source/main.cpp $(LDFLAGS)
	
exec: build/output
	./build/output

compileshaders: shaders/test_shader.vert shaders/test_shader.frag
	glslc shaders/test_shader.vert -o shaders/test_shader.vert.spv
	glslc shaders/test_shader.frag -o shaders/test_shader.frag.spv

clean: 
	rm build/output