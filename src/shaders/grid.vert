R"zzz(
#version 330 core
in vec4 vertex_position;
in vec3 vertex_color;
out vec3 c;
void main() {
    vec4 position = vertex_position;
    gl_Position = position;
    c = vertex_color;
}
)zzz"
