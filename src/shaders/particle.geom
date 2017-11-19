R"zzz(#version 330 core
layout (points) in;
layout (points, max_vertices = 2) out;
void main() {
    vec4 point = gl_in[0].gl_Position;
    
    gl_Position = gl_in[0].gl_Position;
    gl_PointSize = 4;
    EmitVertex();
    EndPrimitive();
//
//    point[0] += 0.00001;
//    gl_Position = point;
//    EmitVertex();
//    EndPrimitive();
}
)zzz"
