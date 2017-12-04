R"zzz(
#version 330 core
in vec4 vertex_position;
in vec4 point_velocity;
out vec4 velocity;
out float weight;
void main() {
    weight = point_velocity[2];
    velocity = normalize(point_velocity);
    velocity[2] = 0;
    gl_Position = vertex_position;
}
)zzz"
