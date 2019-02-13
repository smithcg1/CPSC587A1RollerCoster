Underlying math and physics:
---Arch-length parameterization---
The initial curve is a benzie curve controlled by a set of controll points. This curve this then subdivided repeatedly to get a large number of points that lie on the line. These points are then traversed (each point is looked at in succession) and the distance between points is tallied. When the tally of distance reaches a threshold of deltaS value (0.1), the point is added to the pathing curve. As a result, the curve will be comprised of points that are equidistant from each other so distance can be calculated in later steps.


---Speed calculation---
For the initial starting climb to reach the first peak, the speed is not calculated and instead is locked at a constant so energy can be added to the system in the form of potential energy.

Once the simulation starts at the first peak speed (revered to as vel in the code) is calculated by: s=sqrt(2g(H-h)) where:
    s is the speed
    g is gravity
    H is the highest point on the roller coster
    h is the lowest point on the roller coster

This speed calculation is used to determine how far to move the car along the track.

After the final peak near the bottom speed calculations are turned back off and the cart decelerates by a constant factor which if it came to a complete stop, would allow people to get off but as it is intended to run as a loop, only slows down to a minimum coasting speed.



---Basis vector calculation---
(how the basis is used is described in a later section)

When the relative basis for the cart/track is being calculated, the following values are found:
    lastPoint is a vector that describes the track location just before the active location
    currentPoint is a vector that describes the track location just before the active location
    nextPoint is a vector that describes the track location just after the active location
(how far lastPoint and nextPoint are sampled produce varied amounts of smoothness in the resulting calculation but no exact number is provided here as it is variable)

As these points are a standard distance from each other, they do not indicate the speed the cart would be/is moving at. This means in the following portion the tangent description only accurately represents the direction vector and not the magnitude.

    lastTangent = thisPoint - lastPoint
    thisTangent = nextPoint - thisPoint

To correctly account for the cart speed, each one is normalized to ensure a length of one, then multiplied by the current locations speed.

    lastTangent = normalize(lastTangent) * vel
    thisTangent = normalize(thisTangent) * vel
Although this produces a minor inaccuracy for the lastTangent as the previous point may have had a different speed, the error should be extremely small and so otherwise each now represents an accurate velocity vector.

To calculate the normal, the two tangents are first subtracted as this results in the perpendicular force towards the inside of the osculating circle for the points sampled. This is then added to gravity's force to determine the normal vector.

    normal = (thisTangent - lastTangent) + [0, gravity, 0]

As the bi-normal should be perpendicular to the other two basis, it is calculated as the cross product of the tangent and the normal.

    binormal = thisTangent x normal
    normal = binormal x tangent

The cross product is calculated for bi-normal and normal to ensure the basis vectors are perpendicular and eliminate minor errors in the normal. Once calculated the tangent, normal, and bi-normal are normalized for use in other areas.


---How this affects the track---
As the track is being read in, the lines are drawn with a   +/-  shift from center based on the bi-normal of the basis for that point in the track. This in turn produces a pair of tracks that banks with turns as during a turn there will be a shift in the normal basis direction due to centripetal forces. The tangent basis will match the direction of the track and the bi-normal is perpendicular to both meaning the +/- shift will be perpendicular to the shifted normal and tangent.

---How this affects the cart---
Much like it affects the track, the cart also uses the basis calculations for various points on the track. Each time the cart is rendered, its orientation of the cart is run through a transformation matrix that rotates it based on the locations, tangent, normal, and bi-normal. For the specific matrix please see the code but the result is that the cart faces the direction of the tangent and has its occupants parallel to the forces being applied (reasoning is the same as ---How this affects the track---). The cart is also shifted up slightly along the axis of the normal basis for the point so is sits nicely on the track.


---How FPS is used---
Due to fears of the simulation running too quickly and independent of world time, the program was frame-locked to 60fps. Every 1/60 seconds world time the simulation is asked to update its state for a 1/60 second progression in simulation time. 

The only major change in variables as a result is gravity and speed. As speed is determined solely on gravity and displacement from the peak (see ---Velocity calculation---), only gravity was directly changed. As gravity has a force of 9.81m/(s*s), it was divided by the fps/fps resulting in

9.81m/(sec*sec)	/	60frames/1sec  *   60frames/1sec
9.81m/(sec*sec)	/	360(frames*frames)/ 1(sec*sec)
9.81m/(sec*sec)	*	1(sec*sec) / 360(frames*frames)		(inverse property of division)
9.81m(sec*sec)/360(sec*sec)(frames*frames)
9.81m/360(frames*frames)

This results in an acceleration in meters per frame squared and the velocity calculations are as such in meters per frame. 

This means in each time 1/60 of a second passes, one frame is simulated and the simulated speed and acceleration are updated and accurate for the one frame (1/60 of a second) that has passed.


I apologize for the length of this section but I wanted to be precise and show I know the underlying physics/math of the system. If I have missed anything please let me know.


How to use the program:
Press space to start the simulation
Press "1" to enter free-roam camera mode
Press "2" to enter cinematic camera mode
Press "3" to enter passenger camera mode


Source:
Provided boilerplate code for curve surfing
https://learnopengl.com/In-Practice/2D-Game/Audio    (For audio)
