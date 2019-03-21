// world coord.  of pixel
//varying vec3 frag_position;
void main(void)
{
    //frag_position = gl_Vertex.xyz;
	//3d texture is texture 0.
    gl_TexCoord[0] = gl_MultiTexCoord0;
	// pixel coordinates in screen space.
    gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
}