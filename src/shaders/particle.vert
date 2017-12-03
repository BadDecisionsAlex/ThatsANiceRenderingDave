R"zzz(
#version 330 core
in vec4 vertex_position;
in vec4 point_velocity;
out vec4 velocity;
void main() {
    velocity = normalize(point_velocity);
    gl_Position = vertex_position;
}
)zzz"
