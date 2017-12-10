R"zzz(#version 330 core
layout (points) in;
layout (points, max_vertices = 1) out;

void main() {
    vec4 position = gl_in[0].gl_Position;
//    int size = max(2, min(10, position[2] / 100));
    gl_PointSize = max(2, min(30, position[2]));
    position[2] = 0;
    gl_Position = position;
    EmitVertex();
    EndPrimitive();
}
)zzz"
