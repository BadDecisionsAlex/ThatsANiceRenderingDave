R"zzz(#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
in vec3 c[];
out vec4 color;
void main() {
    
    for (int n = 0; n < gl_in.length(); n++) {
        vec4 position = gl_in[n].gl_Position;
        float alpha = 0;
        if (position[2] != 0) {
            float density = position[2];
            alpha = min(density, 0.9);
            alpha = max(alpha, 0.0);
        } else {
            alpha = 0;
        }
        color = vec4( c[n], alpha );
        
        // Make this another shader
        //color = vec4( c[n], 1.0 );
        //alpha /= 3.0f;
        //if( alpha != 0 )
            //color *= ( 1.0f / alpha );


        position[2] = 0;
        gl_Position = position;
        EmitVertex();
    }
    EndPrimitive();
}
)zzz"
