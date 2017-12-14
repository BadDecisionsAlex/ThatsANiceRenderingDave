R"zzz(#version 330 core
layout (points) in;
layout (points, max_vertices = 1) out;
out vec4 color;

void main() {
    gl_Position = gl_in[0].gl_Position;
    gl_PointSize = 2;
    color = vec4(0.6, 0.0, 1.0, 1.0);
    EmitVertex();
    EndPrimitive();
}
)zzz"
