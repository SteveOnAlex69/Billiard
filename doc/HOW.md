# How did I render 3D stuff

### 0. Opening

So I was reading some linear algebra stuff, when something clicked, and I went straight into coding this.

Overall, this is pretty much a shitpost. Completely unusable on a CV, and I'm not inventing anything new, nor doing things optimally, since I was using something that is meant for 2D stuff for 3D drawing (so, no nice stuff like shader or things like that).

But, I find the process of working the basics of the 3D math pretty fun, so... enjoy the thought process of me trying to make sense of 3D stuff for 3 hours.

### 1. What do I have

I used the SFML library for my Go Game project in my first semester, and I am already used to it, so why not trying to figure out something fun?

In SFML, there is little to no support with 3D stuff. I mean, you can learn OpenGL and mix it with SFML, but I'm too lazy to learn OpenGL, so let's use what I already know:

- `draw_triangle(Point2 a, Point2 b, Point2 c, Color co);`: Draw a triangle of color `co` between point a, b, and c.

This will be our MVP function for this.

### 2. Point and Faces in 3D

So, let's set ourselves a few convention first. Our player will stand at coordinate $(0, 0, 0)$, and will be looking in the Z-direction. In other word, their direction vector will be $(0, 0, 1)$.

Our first problem is this: if there is a point of coordinate $(x, y, z)$, where would it be on our screen?

![alt text](image.png)

*Figure 1: How our field of view and our screen will look like. Sorry for the wrong coordinate.*

Looking at the above figure, the red part is our field of view, and the green part is our camera. 

Notice how when we move toward higher and higher Z, our camera got linearly bigger. So this is our plan: we are going to somehow transform the point, so that it will still be on the same place on the screen, but it would be more comfortable to think about.

The transformation is just:

$(x, y, z) \rightarrow (\frac{x}{z}, \frac{y}{z}, 1)$.

Notice how after the transformation, the Z-coordinate of every point are the same. This mean we can omit the Z-coordinate and be left with 2D coordinate, which are going to be what will be rendered on our screen. Great!

(Note that our 2D coordinate have X and Y value ranges from $-1$ to $1$, but the transform onto our game screen is trivial from this point).

Now we need to draw faces, then draw shapes, then draw terrains, and things like that. Our choice of game to "imitate" is going to be Minecraft, because they only contains cubes, and cubes behave nicely.

We need to draw the face of a cube (I think this is generally called a square). This should not be rocket science, a square is two triangle, and we can draw triangle. So... two triangle draw call.

![alt text](image-1.png)

*Figure 2: A square that consists of two triangles for all of my kindergarten boys out there.*

To draw a cube, we need to draw 6 faces.

Or do we?

Generally speaking, we cannot see the back of a cube, so in reality, only 1-3 faces of a cube is visible to a player. But how do we know which face should we draw? There is a neat technique: Dot product.

In case you haven't graduated middle-school, dot product of two vector generally tell us how are they oriented. In other words, they behaves kind of the same as 
$\cos \widehat{(\mathbf{u}, \mathbf{v})}$.

If two vector make less than a 90 degree angle, their dot product will be positive. If they are perpendicular, their dot product will be 0. Otherwise, their dot product is negative. You can search up how are they computed.

![alt text](image-2.png)

*Figure 3: Our direction vector vs the face normal vector.*

From this, figuring out which faces are visible should be obvious. If the dot product between the normal vector of a face and our direction vector are negative, this means they can see each other.


### 3. Player movement and rotation.

This section is tedious, but not hard.

We have these three attributes: Player position, their yaw (horizontal orientation), and their pitch (vertical orientation). We need to somehow make the 3D object render according to our player position.

Our idea is to translate and rotate all the points, such that in the end, we are at point $(0, 0, 0)$ and is looking at $(0, 0, 1)$. Then we do the camera transformation like above.

First, is the player position. We just subtract the vertex position from the player position. Pretty easy.

Next up is the player orientation, which are their yaw and pitch. Note that doing yaw rotation first, then pitch rotation is not the same as doing pitch first then yaw. These are linear transformation, a.k.a matrix multiplication, and if you graduated middle school, matrix multiplication is not commutative.


Doing these is pretty easy, you just multiply each X, Y, Z by some cosine and sin of yaw and pitch and add or subtract them. One easy check if you are doing it correctly is that the determinant of the transformation matrix has to equal 1 i.e. the volume of our cube is conserved (read what determinant means if you don't know). 

This is an example of an incorrect transformation:

$$
\begin{pmatrix}
\cos(\theta) & 0 & \sin(\theta) \\
0 & 1 & 0 \\
\sin(\theta) & 0 & \cos(\theta)
\end{pmatrix}
$$

This is an example of a correct transformation:

$$
\begin{pmatrix}
\cos(\theta) & 0 & -\sin(\theta) \\
0 & 1 & 0 \\
\sin(\theta) & 0 & \cos(\theta)
\end{pmatrix}
$$

The whole thing blows up without that minus sign.

This is the rough sketch of our transformation. Because we are not doing this with a GPU, we don't need to actually implement matrix multiplication. Writing 3 lines of formula is straight up faster.

```cpp
Point3 externalTransform(Point3 p) {
	p -= player_pos;
	Point3 u(p.x * cos(yaw) - p.z * sin(yaw), 
        p.y, 
        p.z * cos(yaw) + p.x * sin(yaw));
	Point3 v(u.x, 
        u.y * sin(pitch) - u.z * cos(pitch), 
        u.z * sin(pitch) + u.y * cos(pitch));
	return v;
}
```

Our 3D is basically done, we successfully draw a cube!

### 4. Polishing stuff

So is that it? Did we conquer Japan? Not yet.

![alt text](image-3.png)

*Figure 4: A very ugly looking cube.*

We have to somehow apply lighting and shading to the cube. Luckily, there is an easy way to do this using dot product.

Using the same logic as the last section, we can tell if a face is facing towards or away from the light source. We simply just have to calculate the dot product of the light ray and the surface normal vector, and offset by some amount to the color space.

```cpp
Point3 light_cast(-0.5, -2, -1);
for (int f = 0; f < 6; ++f) {
    float light_intensity = dotProduct(light_cast, normal[f]);
    float brightness = 140 - light_intensity * 40;
    sf::Color cur = sf::Color(brightness, brightness, brightness);
}
```

![alt text](image-4.png)


*Figure 5: A very handsome looking cube.*

So we generated one cube. How do we generate multiple cube? Generate multiple cube is harder, since we have to know which face to generate first.

The naive solution would be to make all the faces, then sort them by distance to the player. This runs in $O(F \log F)$, which is excruciatingly slow. However, as we have mentioned before, cube behaves nicely, and so we can generate blocks by their distance to the player. Any distance function will work.

Here is how the code looks like.


```cpp
std::vector<int> orderX, orderY, orderZ;
for (int i = 0; i < W; ++i) {
	orderX.push_back(i);
	orderY.push_back(i);
	orderZ.push_back(i);
}

std::sort(ALL(orderX), [](int a, int b) {
	return std::abs(player_pos.x - a) > std::abs(player_pos.x - b);
	});
std::sort(ALL(orderY), [](int a, int b) {
	return std::abs(player_pos.y - a) > std::abs(player_pos.y - b);
	});
std::sort(ALL(orderZ), [](int a, int b) {
	return std::abs(player_pos.z - a) > std::abs(player_pos.z - b);
	});

for (int i: orderX)
	for (int j: orderY)
		for (int k: orderZ) 
			if (world.get_block_type(i, j, k))
				draw_cube(Point3(i, j, k));
```

So we can draw multiple cube in $O(F)$ now. That's great, but our program performance still sucks.

Here is a few optimization tricks:

- If a face is facing another block, you can skip generating the face.
- Load everyone into the buffer at once, then use a single draw call. If you create a buffer and draw call for every single triangle, then it would be a shit load of overhead.'

This should speed up our program by an order of magnitude. I think I can optimize this a bit more, but I'm too tired.

### 5. Player interaction.

We will also implement breaking and placing block.

I'm too lazy to work out the optimized math for really long range. Luckily, Minecraft only allow reaching 5 blocks, so I'm thankful.

My solution is literally just look through blocks with distance of less than 5 from the player, then check if the player can see one of the face of the block.

To check if the player can see that face, you can convert the face into the 2D plane, then use cross product to determine if the triangle contains $(0, 0)$.

For breaking block, the target block would be of the face we are looking at. For placing block, the target block would be the block that the face we are looking at are facing.

### 6. Epilogue

I don't know, I made this in 3 hours. I left my own thinking process here, because I find figuring out things for yourself very cool and fun.