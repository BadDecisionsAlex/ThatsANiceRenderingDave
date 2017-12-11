R"zzz(#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
out float alpha;
void main() {
//    vec4 position = gl_in[0].gl_Position;
//    int size = max(2, min(10, position[2] / 100));
//    gl_PointSize = max(2, min(30, position[2]));
//    position[2] = 0;
//    gl_Position = position;
//    EmitVertex();
//    EndPrimitive();
    
    for (int n = 0; n < gl_in.length(); n++) {
        vec4 position = gl_in[n].gl_Position;
        
        if (position[2] != 0) {
            float density = position[2];
            
            alpha = min(density / 15, 0.9);
        } else {
            alpha = 0;
        }
        
        position[2] = 0;
        gl_Position = position;
        EmitVertex();
    }
    EndPrimitive();
}
)zzz"
