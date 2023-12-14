# shadow-garden CSCI 1230 Final Project

## README

A high-level **overview** of the flow of your final program:

Our program consists of three new functionality for the OpenGL realtime scene rendering:

1. New Scenefiles with Meshes inclusion:
Apart from getting the new mesh files as a scenefiles, we implement new approach to parse the mesh files due to the difference of the mesh file format of the external files.

2. Light Position/Direction Changes based on time:
We simply use the same idea as the mouse movement, which is converting the light direction into some angle. Then, we make the time scale to determine the angle from the plane (in our case, we decide to use 24 hours as a basis). For the automatic, we use the time notion for changing the angle.

3. Shadow Mapping:
Our idea is including the shadow mapping for the realtime pipeline, which will focus only in the directional lights. As we learn about the idea of shadow mapping in the OpenGL, we learn that the concept is as follow:

- Rendering the "depthMap" from the light's point of view (everything the light cannot see is the shadow) as the first pass
- Rendering the normal scene from the camera's point of view (normal scene rendering) as the second pass

With this idea, we have three main portions of this work:
1. Creating the depth map :  The depth map is the depth texture as rendered from the light's perspective that we'll be using for testing for shadows.
2. Light Space Transform and Rendering to depth map : Simply settings the light view based on the view matrix and projection matrix. Then, convert them into the depth map using the shaders. 
3. Grouping everything into Shadow Mapping : Operates everything on the fragment shaders to get the correct light based on the position.

**Functionality in the code**:
1. To activate the new meshes, simply importing the new mesh file into the GUI in the Action assignment
2. To activate the Light position:
- Move the slider saying the "time" around. The number represents the time in the day (0.00 - 24.00)
- To automate this process, click "day-night cycle" to make this process works
3. To activate the Shadow Mapping, click the button "Shadow-Enabled" to make it.
  
**Division of labor** and a rough **plan of action**
    - **Naphat** : Working on the Shadow Mapping
    - **Nop** : Working on the Light Position/Direction Changes based on time
    - **Treetased** : New Scenefiles with Meshes inclusion
    
A description of **how each feature will be implemented** + any **resources** that you think might come in handy (e.g. papers, tutorials, algorithms, data)
	- https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping (For the Shadow Mapping)
	- Assignment 5 and 6 in terms of GL Pipeline
	- OpenGL Documentation
 	- EdStem
