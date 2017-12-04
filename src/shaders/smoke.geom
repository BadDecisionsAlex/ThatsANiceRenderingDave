R"zzz(#version 330 core
layout (points) in;
layout (points, max_vertices = 100) out;
in vec4 velocity[];
in float weight[];
out float alpha;
out vec4 color;

float size = 20;
float half_size;

void addPoint(vec4 point, int depth) {
    float x = ((2.0f * point[0]) / float(2000)) - 1.0f;
    float y = ((2.0f * point[1]) / float(2000)) - 1.0f;
    vec4 p = vec4(x, y, point[2], 1.0);
    
    //max depth = 5
    alpha = (min(depth, 3) / 50.0) + 0.01;
    
    if (depth == 5) {
        color = vec4(0.7, 0.7, 0.7, alpha);
    } else if (depth == 4) {
        color = vec4(0.65, 0.65, 0.65, alpha);
    } else if (depth == 3) {
        color = vec4(0.6, 0.6, 0.6, alpha);
    } else if (depth == 2) {
        color = vec4(0.57, 0.52, 0.6, alpha);
    } else {
        color = vec4(0.5, 0.5, 0.6, alpha);
    }
    
    int point_size =  2 * int((size + 2) * (1.0 / depth));
    
    gl_Position = p;
    gl_PointSize = point_size;
    EmitVertex();
    EndPrimitive();
}

void expandUp(vec4 point, int depth) {
    
    float scale = (-velocity[0][1] / 2 + 0.75);
    
    for (int i = 0; i < depth; ++i) {
        point[1] += half_size * scale * scale;
        addPoint(point, depth - i);
    }
}

void expandDown(vec4 point, int depth) {
    
    float scale = (velocity[0][1] / 2 + 0.75);
    
    for (int i = 0; i < depth; ++i) {
        point[1] -= half_size * scale * scale;
        addPoint(point, depth - i);
    }
}

void expandLeft(vec4 point, int depth) {
    
    float scale = (velocity[0][0] / 2 + 0.75);
    
    for (int i = 0; i < depth; ++i) {
        point[0] -= half_size * scale * scale;
        addPoint(point, depth - i);
        
        expandUp(point, depth - i - 1);
        expandDown(point, depth - i - 1);
    }
}

void expandRight(vec4 point, int depth) {
    
    float scale = (-velocity[0][0] / 2 + 0.75);
    
    for (int i = 0; i < depth; ++i) {
        point[0] += half_size * scale * scale;
        addPoint(point, depth - i);
        
        expandUp(point, depth - i - 1);
        expandDown(point, depth - i - 1);
    }
}


void main() {
    vec4 point = gl_in[0].gl_Position;
    int depth = 5;
    
    size = weight[0];
    half_size = size / 2.0;
    
    addPoint(point, depth);
    expandLeft(point, depth);
    expandRight(point, depth);
    expandUp(point, depth);
    expandDown(point, depth);
}
)zzz"
