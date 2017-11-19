R"zzz(#version 330 core
layout (points) in;
layout (points, max_vertices = 100) out;
void main() {
    vec4 point = gl_in[0].gl_Position;
    
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    EndPrimitive();
    
    for (int x = 0; x < 99; x++) {
        point[0] += 1;
        gl_Position = point;
        EmitVertex();
        EndPrimitive();
    }
}
)zzz"
