R"zzz(
#version 330 core
in vec4 vertex_position;
in float bone_id;
out float vs_bone_id;
out float vs_selected_bone;
void main() {
    gl_Position = vertex_position;
    vs_bone_id = bone_id;
}
)zzz"
