

// computes a simplified lighting equation
//varying vec3 frag_position;
uniform sampler3D volume_texunit;
uniform sampler1D trfunc_texunit;

void main(void)
{
	vec4 color = texture1D(trfunc_texunit, texture3D(volume_texunit, gl_TexCoord[0].xyz ).r);
	gl_FragColor = color;
}