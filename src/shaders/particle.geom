R"zzz(#version 330 core
layout (points) in;
layout (points, max_vertices = 2) out;
out vec4 color;
void main() {
    vec4 point = gl_in[0].gl_Position;
    
    color = vec4((point[0] + 1) / 2, (point[1] + 1) / 2, 0.0, 1.0);
    
    gl_Position = gl_in[0].gl_Position;
    gl_PointSize = 12;
    EmitVertex();
    EndPrimitive();
}
)zzz"
