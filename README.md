# Basic algorithm Graphics
This is a repository of some basic algorithm written in `C/C++` and using some Extend Library like `libigl`, `CGAL`, `OpenGL` and etc.


## AABB-ray and AABB-segment slab method

- CGAL
- OpenGL

Slab method is used to detect the intersection of the **bounding box** and the **ray**. On this basis, I only modify a very small part of the code, so that the algorithm can be used for **Segment** detection.

In this code, `AABB_is_hit_ray` and `AABB_is_hit_segment` functions are major algorithm (They are pretty the same), I use C/C++ and CGAL to create **Segment**, **Ray**, **AABB** and etc, use OpenGL to display the above geometry:

![](.\AABB-ray_segment-slab-method\display.png)

## SAT - Separating Axes Theorem (Take cubes and triangles as an example)

- OpenGL

This algorithm is used to quickly determine the intersection of a cube and a triangle face. `In fact, SAT can be used to judge the intersection of many polygons`.

As follows(`red` is cube, `blue` is triangle intersects cube, `white` is triangle that does not intersect with cube):

![](.\SAT_Triangle_Cube\picture_0.png)