# Table of Contents

- [Basic Milestones](#basic-milestones)
- [Extra Additions](#extra-additions)
- K-D Tree
- Super Sampling
- Transmission Shadow

# Summary

Devs: Zoe Wheatcroft

C++ physics based ray tracing engine-- in progress! 

code [here](https://github.com/ZoeWheatcroft/PhysicalRayTracingEngine)

# Basic Milestones
## **Refraction**
![refraction](https://github.com/user-attachments/assets/43b7ca9d-60d0-4e83-97e5-715e0964d0ff)
![non-refractive](https://github.com/user-attachments/assets/cbc6936c-f971-476b-97e1-8f9f64bee4fb)
The same scene with a refractive and non-refractive first ball. I also modified the shadow ray test to take transmissive materials into account-- I didn't realize that not only would a glass ball, as in this case, not have a shadow, but it also has _two_ specular highlights. 

## **Reflection**

![image](https://github.com/user-attachments/assets/e5afe800-7143-4cfc-b5ff-61c73fc7697c)

## **Procedural Textures**

![procedural_textures](https://github.com/user-attachments/assets/f7231305-6b10-4ce0-a0ff-70f257a74dbe)

## **Phong Lighting**

![phong](https://github.com/user-attachments/assets/0653795e-fc79-44c3-ac03-f7c4ba787570)

## **Basic Image!**

![ray_tracing](https://github.com/user-attachments/assets/7dae40e3-2d98-4a6e-b61d-a653d7fc7eff)

## **Setting the Scene**

Here is my Blender recreation of Whitted's sample scene: 
![image](https://github.com/user-attachments/assets/6b49ea53-3ef2-499a-972b-2f8d33353361)

It is as faithful of a recreation that I could manage! It may be tiny bit off but it achieves a semblance. 
Here are its measurements (location in x,y,z format, y-up is positive): 
* Center sphere
  * size: radius of 1
  * location: (0, 1.03, -4.4)
* Back sphere
  * size: radius of 1
  * location: (0.87, 0.6, -2.0)
* Floor
  * size: (0.6, 0, 2.0)
  * location: (1.3, 0, 0)
* Camera
  * width: 1, height 0.5625
  * virtual viewpoint: (0, 1, -7.8)
  * rotation: (0, 0, 0)




# Extra Additions

## **K-D Tree**
![tiny_bunny_kdtree](https://github.com/user-attachments/assets/8e254bec-79f9-4b21-81a1-9e5e3e1680ea)

Above is the Stanford Bunny in my ray tracer. With this version of the bunny, there are 948 faces and 453 vertices. With the K-D tree, the entire image took just about 200 seconds to render. WIthout the k-d tree, it took almost 500. What a testament to the necessity of the k-d tree for high poly scenes!


## **Supersampling**

With the supersampling (4 rays per pixel):
![image](https://github.com/user-attachments/assets/c44b8178-c843-4e07-b3ae-43fc48b14d21)

Without the supersampling:
![image](https://github.com/user-attachments/assets/4c5103e6-6461-4256-a027-cc28b3f59bf7)

## **Transmissive material shadow**
![refraction](https://github.com/user-attachments/assets/43b7ca9d-60d0-4e83-97e5-715e0964d0ff)

