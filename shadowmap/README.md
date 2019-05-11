## Shadow Mapping

I implement the effect of shadowing using shadow mapping algorithm using OpenGL.

### Reference
To overcome the perspective aliasing and produce soft shadows, I apply *CSM* algorithm and *PCF* to this program. Following are the links to the articles I referred to.

- [Basic Concepts of Shadow Mapping](https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping)

- [CSM and PCF](https://docs.microsoft.com/en-us/windows/desktop/dxtecharts/cascaded-shadow-maps)

### Pipeline

Following is the steps needed to render the scene with shadows.

- Initialize the environment of 

- Load the Models and Textures
- Initialize Vertex Array Objects and Buffers
- Initialize OpenGL programs for rendering

- Render multiple shadow maps mentioned in *CSM* (I use the Fit to Cascade strategy mentioned in article)
- Render the GBuffer for deferred lighting
- Render a full-screen quad and calculate the lighting

## Existing Problems

- Need to compute depth bias adaptively
- Need to partition the view frustum adaptively
- Above introduces to shimmering artifacts

## Scene
PCF kernel size: 7 x 7

Resolution of Shadow Map(4 level CSM): 2048 * 2048, 1536 * 1536, 1024 * 1024, 512 * 512



![far](https://github.com/UI233/OpenGLplayground/tree/master/shadowmap/img/far.png)



![closerlook](https://github.com/UI233/OpenGLplayground/tree/master/shadowmap/img/closerlook.png)





