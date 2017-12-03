R"zzz(#version 330 core
layout (points) in;
layout (points, max_vertices = 100) out;
in vec4 velocity[];
out float alpha;
out vec4 color;

void addPoint(vec4 point, int depth) {
    float x = ((2.0f * point[0]) / float(2000)) - 1.0f;
    float y = ((2.0f * point[1]) / float(2000)) - 1.0f;
    vec4 p = vec4(x, y, point[2], 1.0);
    
    //max depth = 5
    alpha = (depth / 25.0) + 0.2;
    
    int size = depth * depth + 2;
    
    gl_Position = p;
    gl_PointSize = size;
    EmitVertex();
    EndPrimitive();
}

void expandUp(vec4 point, int depth) {
    int size = 15;
    
    float scale = (-velocity[0][1] / 2 + 0.75);
    
    for (int i = 0; i < depth; ++i) {
        point[1] += size * scale * scale;
        addPoint(point, depth - i);
    }
}

void expandDown(vec4 point, int depth) {
    int size = 15;
    
    float scale = (velocity[0][1] / 2 + 0.75);
    
    for (int i = 0; i < depth; ++i) {
        point[1] -= size * scale * scale;
        addPoint(point, depth - i);
    }
}

void expandLeft(vec4 point, int depth) {
    
    int size = 15;
    
    float scale = (velocity[0][0] / 2 + 0.75);
    
    for (int i = 0; i < depth; ++i) {
        point[0] -= size * scale * scale;
        addPoint(point, depth - i);
        
        expandUp(point, depth - i - 1);
        expandDown(point, depth - i - 1);
    }
}

void expandRight(vec4 point, int depth) {
    int size = 15;
    
    float scale = (-velocity[0][0] / 2 + 0.75);
    
    for (int i = 0; i < depth; ++i) {
        point[0] += size * scale * scale;
        addPoint(point, depth - i);
        
        expandUp(point, depth - i - 1);
        expandDown(point, depth - i - 1);
    }
}


void main() {
    vec4 point = gl_in[0].gl_Position;
    int depth = 5;
    
    addPoint(point, depth);
    expandLeft(point, depth);
    expandRight(point, depth);
    expandUp(point, depth);
    expandDown(point, depth);
}
)zzz"
