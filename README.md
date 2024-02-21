# Spaceview
My first project made in OpenGL. 
<img width="1278" alt="image" src="https://github.com/Mariusacg/Spaceview/assets/116760747/acf14216-ba8f-4532-b1f5-b09a7c7ae285">


## Blinn-Phong model and Shadow-mapping
The Blinn-Phong model is a widely used shading model in modern computer graphics, renowned for its simplicity and efficiency in approximating realistic lighting.
This model computes the reflection of light on a surface by combining ambient, diffuse, and specular reflections. Ambient reflection represents the constant background light present in a scene, while diffuse reflection simulates the scattered light on a surface, which depends on the angle between the surface normal and the light source direction. The specular reflection accounts for the shiny highlight on surfaces, determined by the viewer's position and the surface's roughness.

Shadow mapping is also a popular technique that involves rendering the scene from the perspective of a light source, and storing the result in a depth map to capture the distances of objects from the light source. This depth information is then used to determine which areas of the scene are in shadow and which are illuminated.

During the final rendering, each pixel's position is compared with the corresponding position in the depth map. If the depth of the pixel in the scene is greater than the depth stored in the depth map, it means that the pixel is behind an object and is therefore in shadow.

<img width="1275" alt="image" src="https://github.com/Mariusacg/Spaceview/assets/116760747/e89372f7-a3f7-4e16-b265-b5e14f301f5d">

## Multi-functional spaceship

The spaceship boasts full maneuverability along all three axes, affording it the capability to navigate in any direction with ease, can also rotate on it's y-axis and can perform this actions:

### Light-up a whole area(point-light)  
<img width="1279" alt="image" src="https://github.com/Mariusacg/Spaceview/assets/116760747/367d0dec-0f78-422c-9d2c-6d86f6635bc2">

### Can shoot unlimited bullets from head-mounted twin cannons

<img width="1280" alt="image" src="https://github.com/Mariusacg/Spaceview/assets/116760747/d5678320-a435-47a1-b518-7cae3ee2e6b9">

