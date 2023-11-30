# shadow-garden
CSCI 1230 Final Projects 

## First Check-in

- A high-level **overview** of the flow of your final program:
Our idea is including the shadow mapping for the realtime pipeline, which will focus only in the directional lights. 

As we learn about the idea of shadow mapping in the OpenGL, we learn that the concept is as follow:

We render the scene from the light's point of view and everything we see from the light's perspective is lit and everything we can't see must be in shadow. Imagine a floor section with a large box between itself and a light source. Since the light source will see this box and not the floor section when looking in its direction that specific floor section should be in shadow.

Shadow mapping therefore consists of two passes: first we render the depth map, and in the second pass we render the scene as normal and use the generated depth map to calculate whether fragments are in shadow. It may sound a bit complicated, but as soon as we walk through the technique step-by-step it'll likely start to make sense.

With this idea, we have three main portions of the work:
1. Creating the depth map :  The depth map is the depth texture as rendered from the light's perspective that we'll be using for testing for shadows.
2. Light Space Transform and Rendering to depth map : Simply settings the light view based on the view matrix and projection matrix. Then, convert them into the depth map using the shaders. 
3. Grouping everything into Shadow Mapping : Operates everything on the fragment shaders to get the correct light based on the position.
- **Division of labor** and a rough **plan of action**
    - **Naphat** : Creating the depth map, and cooperate in Shadow Mapping. The plan is following the idea in the Shadow Mapping website, cooperating with the result in the Action assignment. If this part is not finished, we may using some other specific to shape or static depth map to display
    - **Nop** : Light Space Transform and Rendering to depth map and cooperate in Shadow Mapping. The idea is using the idea of converting the view and projection matrix from the Lights assignment to work on the lights, then using the depth map to cast the constant. If this part is not finished, we could create a simpler scene that doesn't require much of the shift in view and projection matrix to work
    - **Treetased** : Mainly work on the Shadow Mapping in terms of the cooperation with the code. The idea is laying down the backbone of the shadow algorithms based on the website in shadow mapping, then trying to find where the depth map and light space transformation could come into the place. If this process is unfinished, we could also make a more simple scene that ignore some notions or edge cases of the shapes.
- A description of **how each feature will be implemented** + any **resources** that you think might come in handy (e.g. papers, tutorials, algorithms, data)
	- https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping (For the Shadow Mapping)
	- Assignment 5 and 6 in terms of GL Pipeline
	- OpenGL Documentations
