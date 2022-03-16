#version 140
uniform sampler2D sampler;
uniform sampler2D correction;
uniform vec4 modulation;
uniform vec2 inv_screenres;
uniform float gain;
uniform float saturation;

in vec2 texcoord0;
in vec4 gl_FragCoord;
out vec4 fragColor;

float rand(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 from_srgb(vec3 x)
{
    vec3 mask = clamp((x - 0.04045) * 200.0, 0.0, 1.0);
    vec3 xlinear = x * 0.0773993808049;
    vec3 xgamma = pow((x + 0.055) * 0.947867298578, vec3(2.4));
    return mix(xlinear, xgamma, mask);
}

vec3 to_srgb(vec3 x)
{
    vec3 mask = clamp((x - 0.0031308) * 200.0, 0.0, 1.0);
    vec3 xlinear = x * 12.92;
    vec3 xgamma = 1.055 * pow(x, vec3(0.416666666667)) - 0.055;
    return mix(xlinear, xgamma, mask);
}

void main()
{
    vec4 tex = texture(sampler, texcoord0);
    vec3 lcorr = from_srgb(texture2D(correction, gl_FragCoord.xy * inv_screenres.xy).rgb);

    tex *= modulation;
    tex.rgb = from_srgb(tex.rgb);

    lcorr = lcorr * strength + offset;
    lcorr = 1.0 / clamp(lcorr, 0.1, 1.0);

    // Add some random noise to prevent quantization aliasing
    tex.rgb *= lcorr * gain;
    tex.rgb = to_srgb(tex.rgb) + rand(gl_FragCoord.xy) * 0.004;

    fragColor = tex;
}
