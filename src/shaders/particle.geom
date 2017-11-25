R"zzz(#version 330 core
layout (points) in;
layout (points, max_vertices = 2) out;
void main() {
    vec4 point = gl_in[0].gl_Position;
    
    gl_Position = gl_in[0].gl_Position;
    gl_PointSize = 6;
    EmitVertex();
    EndPrimitive();
}
)zzz"
