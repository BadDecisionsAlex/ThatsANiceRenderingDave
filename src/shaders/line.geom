R"zzz(#version 330 core
layout (lines) in;
layout (line_strip, max_vertices = 102) out;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform float selected_bone;
uniform vec4 selected_n;
uniform vec4 selected_b;
in float vs_bone_id[];
out float bone_id;

void createLine(vec4 point_a, vec4 point_b, float id) {
    gl_Position = projection * view * model * point_a;
    bone_id = id;
    EmitVertex();
    
    gl_Position = projection * view * model * point_b;
    bone_id = id;
    EmitVertex();
    
    EndPrimitive();
}

void main() {
    mat4 pvm = projection * view * model;
    vec4 start = gl_in[0].gl_Position;
    vec4 end = gl_in[1].gl_Position;
    
    for (int n = 0; n < gl_in.length(); n++) {
        bone_id = vs_bone_id[n];
        gl_Position = pvm * gl_in[n].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
    
    if (vs_bone_id[0] == selected_bone) {
        vec4 n = normalize(selected_n);
        vec4 b = normalize(selected_b);
        createLine(start, start + n, -2);
        createLine(start, start + b, -3);
        
        int numberOfLines = 16;
        float angle = (2 * 3.14159) / numberOfLines;
        vec4 previous = vec4(0.0f, 0.0f, 0.0f, 0.0f);
        for (int x = 0; x <= numberOfLines; x++) {
            float t = angle * x; //Theta
            vec4 l = normalize(start - end); //Line
            float r11 = l[0] * l[0] + (1 - l[0] * l[0]) * cos(t);
            float r21 = l[0] * l[1] * (1 - cos(t)) - l[2] * sin(t);
            float r31 = l[0] * l[2] * (1 - cos(t)) + l[1] * sin(t);

            float r12 = l[0] * l[1] * (1 - cos(t)) + l[2] * sin(t);
            float r22 = l[1] * l[1] + (1 - l[1] * l[1]) * cos(t);
            float r32 = l[1] * l[2] * (1 - cos(t)) - l[0] * sin(t);

            float r13 = l[0] * l[2] * (1 - cos(t)) - l[1] * sin(t);
            float r23 = l[1] * l[2] * (1 - cos(t)) + l[0] * sin(t);
            float r33 = l[2] * l[2] + (1 - l[2] * l[2]) * cos(t);
            mat4 rotation = mat4(vec4(r11, r21, r31, 0),
                                 vec4(r12, r22, r32, 0),
                                 vec4(r13, r23, r33, 0),
                                 vec4(0, 0, 0, 1));

            vec4 point = rotation * n * 0.5;
            if (x > 0) {
                createLine(start + previous, start + point, -5);
                createLine(end + previous, end + point, -5);
                createLine(start + point, end + point, -5);
            }

            previous = point;
        }
    }
}
)zzz"
