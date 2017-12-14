R"zzz(
#version 330 core
in vec4 velocity;
void main(){
    gl_Position = velocity;
}
)zzz"
