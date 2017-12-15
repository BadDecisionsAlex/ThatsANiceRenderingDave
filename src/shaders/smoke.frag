R"zzz(
#version 330 core
in float alpha;
in vec4 color;
out vec4 fragment_color;
void main() {
    fragment_color = color;
}
)zzz"
