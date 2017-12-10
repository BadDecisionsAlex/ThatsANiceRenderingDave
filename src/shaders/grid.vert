R"zzz(
#version 330 core
in vec4 vertex_position;
void main() {
    vec4 position = vertex_position;
    gl_Position = position;
}
)zzz"
