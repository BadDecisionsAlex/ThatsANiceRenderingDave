R"zzz(
#version 330 core
in float alpha;
in vec4 color;
out vec4 fragment_color;
void main() {
    fragment_color = color;//vec4(0.5, 0.5, 0.6, alpha);
}
)zzz"
