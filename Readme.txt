Underlying math and physics:
---Arch-length paramatization---
The initial curve is a benzie curve controlled by a set of controll points.
This curve this then subdivided repetedly to get a large number of points that lie on the line.
These points are then traversed and the distance between points is tallied.
When the tally reaches a threshhold deltaS value (0.1), the point is added to the pathing curve.


---Velocity calculation---
Speed is calculated by s=sqrt(2g(H-h)) where:
    s is the speed
    g is gravity
    (H-h) is the change in hight 

This is then multiplied by the basis vector tangent to the track to get velocity.



---Basis vector calculation---
The basis vecors are are calculated such that the:
    lastTangent = thisPoint - lastPoint
    thisTangent = nextPoint - thisPoint

    normal = (thisTangent - lastTangent) - [0, gravity, 0]
    binormal = thisTangent x normal
    normal = binormal x tangent

Once calculated the tangent, normal, and binormal are normalized.
Crossproduct is calculated for binormal and normal to ensure the basis vectors are perpendicular.



---How this affects the track---
As the track is being read in, the lines are drawn with a +/- shift from center based on the binormal
The track is also shifted down by a bit relative the the normal







How to use the program:
Press space to start the simulation
Press "1" to enter free-roam camera mode
Press "2" to enter cinematic camera mode
Press "3" to enter passenger camera mode


Source:
Provided boilerplate code for curve surfing
