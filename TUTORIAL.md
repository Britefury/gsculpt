# Introductory tutorial

##  Viewport navigation

There are three modes of navigation; rotation/orbiting, panning, and zooming.

**Rotation/orbiting:** To rotate/orbit the viewport, hold the `Alt`-key, press and hold the left mouse button, and move the mouse.

**Panning:** To pan the viewport, hold the `Alt`-key, press and hold the middle mouse button, and move the mouse.

**Zooming:** To zoom the viewport, hold the `Alt`-key, press and hold the right mouse button, and move the mouse 
left and right.

## Creating a box primitive

In this section we will create a box primitives. There are two ways to create primitives; a long way, and a short way. The long way will be used to create the box, in this section. The short way for the sphere, in the next section

**Creating the node:** In the tool panel on the left of the screen, you should see a tab at the top called *Scene*. This tab should be active. Below it, you should see two buttons, one called *Choose model*, the other called *Scene graph*. Press the *Scene graph* button. The *Scene graph* window should appear. Click the *New* button at the top left of the scene window. A menu will appear. Choose *Blank model*. You will be prompted for a name. Type *Box*. Press enter, or click the OK button. A node called *Box* will appear in the scene graph area of the scene window. Double click it to edit it. (You can also single-click it, and then press the *Edit* button on the left of the *Scene graph* window to achieve the same thing) You may now close the scene graph window.

**Creating the box primitive:** In the left panel in the main window, a new tab called *Prim* will have appeared. Click this tab to bring up the primitive tools. Click the *Box* button. The *Prim* tab will have been replace by a tab called *Mesh*. A new tab called *Box* will appear and be selected automatically. In it you will find the parameters for the box tool. Change them if you wish. A box will have appeared in the viewport, along with a red, green, and blue manipulator. In the tool panel, you should see options for the box tool. These are: the number of divisions in *X*, *Y*, and *Z* axes, the size, the position, and 6 check boxes which can turn the faces of the box on and off. Finally, at the bottom, there is a *Done* button, which will dismiss the tool. This can also be activated using the `\` key. We want to move the box 50 units to the left. To do this, enter *-50* into the *X position* entry box, and press `Enter`. The box should move. Dismiss the box tool using the *Done* button, or the `\` key.

## Creating a sphere primitive

Click the *Create* menu in the menu bar at the top, and choose *Sphere*. This will create a new node for the sphere model. This creates the node (what you did in the scene graph window in the previous section), and activates the sphere primitive tool. You should now see three tabs in the tool panel; *Scene*, *Mesh*, and *Sphere*. The *Sphere* tab should be active. A sphere will have appeared in the viewport, in addition to the box.

## Moving the sphere using the manipulator

With the *Sphere* tool still active from the last section, you should see a sphere in the viewport. At the centre of the sphere, you should see a manipulator with long red, green, and blue handles, smaller yellow, cyan, and magenta corner handles. You may move the sphere by pressing and holding the left mouse button in the viewport, and dragging. Moving the pointer over the manipulator while the button is not held will cause the manipulator handles to highlight as the pointer passes over them. Starting your click-drag on the red, green, or blue handles will constrain the motion of the sphere to the *X*, *Y*, and *Z* axes respectively. Starting on the yellow, cyan, or magenta handles will constrain motion to the *XY*, *YZ*, and *XZ* planes respectively. Starting anywhere else in the viewport will produce unconstrained motion.

**Grid snapping:** Holding `shift`, `control`, or `shift`+`control` while dragging will snap the position to points on a grid. The size of the grid depends upon which key combination is held, and the snap settings. The snap settings are accessible via the *snap settings* button in the toolbar (its the one with the blue grid). Clicking the *snap settings* button will cause the snap settings to pop up, from which you can control the snap sizes for distance/position, scale/fraction, and angle/rotation, for the various key combinations.

Use the manipulator, with grid snapping if you wish, to move the sphere 50 units to the right. You may also enter 50 into the *X* position entry.

## Rotating the box

Click the *Scene* tab at the top of the tool panel. Now click the *Choose model* button (default shortcut: `control`-`space`). A new tab will have appeared, titled *Choose model*. This allows you to pick a different model to edit. Click on the box. The box will now become the current model. Dismiss the *Choose model* tool with the *Done* button. (Or the `\` key)

Click the *Mesh* tab at the top of the tool panel. There are four buttons at the top of the mesh panel. These allow you to choose which component type you wish to edit. They are for (from left to right) *vertex mode*, *edge mode*, *face mode*, and *body mode*. Click the *body mode* button. In the *Transformations/Deformations* section, you will find the *Rotate* button. If you click the right mouse button in the viewport, the context menu will appear. The *Deformations* sub-menu will contain a *Rotate* item for the rotate tool. Use either the *Rotate* button in the tool panel, or the context menu to activate the rotate tool. The rotate tool parameters will appear in a new tab in the tool panel, and a rotation manipulator will appear in the viewport. The red, green, and blue rings allow you to rotate around fixed axes. You can rotate the shiny blue transparent sphere part of the manipulator by dragging it. Dragging anywhere else in the viewport results in a free-hand rotation. Once again, grid snapping can be used by holding `shift`, `control`, or `shift`+`control`. Rotate the box until you are happy.



# Using gSculpt project files as modelling tutorials

## Viewing a gSculpt modelling tutorial

Making and viewing modelling tutorials is one of gSculpts' main strengths.

Lets use the head model that comes as one of the examples in the gSculpt package. Go to the *File* menu and choose *Open*. From the *gSculpt* directory (on Windows, it is probably `C:\Program Files\Britefury\gSculpt`), find the file `head.gpx`, which can be found in the `examples` directory. Open it.

You should see a head model appear in your viewport. It is currently in a background layer. You will need to edit the head model. Ensure that the *Scene* tab (in the toolbar on the left) is selected, and click the *Scene* button to bring the scene editor window up. You should see a node called *head* in the scene editor window. Double-click it to edit it (or click it to select it, and click the *Edit* button on the left of the scene editor window). Close the scene editor window.

In the scene tab, below the *Scene* button that you just used, you will see a list that is titled *Plan*. This shows all the operations required to build the model from scratch. Scroll the list to the top. The list will start with an entry called *Node: Model*. Below this, will be an entry called *Box*. Click the box entry to select it. You will see a box instead of a head in the viewport. You may now press the `down-arrow` cursor key to move to the next entry in the list. Holding the `down-arrow` key will scan through the plan, action by action. Using the `up-arrow` will scan in the opposite direction. Using the `page-down` and `page-up` keys will move a page at a time. This way you can effectively play back the construction of the model, and rewind as you choose. You can also use the navigation controls (Hold `Alt`-key, press and hold a mouse button (left for rotate, middle for pan, right for zoom), and move the mouse) to view the model from the perspective of your choosing.

The head model also includes a model of the eyes. You may wish to hide them in order to prevent them from obscuring your view of the main head model. In order to do this, click the *layers* button in the tool bar at the top of the gSculpt window. Its the 4th button from the left. The *Layers* window will appear. You should see a layer called *Default*. Expand it to reveal two entries, one called *head*, the other called *eyes*. These correspond to the nodes in the scene editor window. To hide them, uncheck the check-box in the *Vs* column of the layer list.

## Creating a gSculpt modelling tutorial

Creating a modelling tutorial in gSculpt is easy. There is no need to save your progress at various stages into different files as you go. gSculpt records each modelling action as you take it.

Create your model in the normal way, and distribute the gSculpt project file. A person wishing to learn your technique will be able to do so by playing your technique back in the same way as described in the previous section.

If you wish to take screen shots, you do not need to do this as you create your model. Create your model in the normal way. Use the technique presented in the previous section to choose an appropriate point in the construction history, choose your perspective, and take a screen shot using screen-grabber software.


# Using live subdivision

Live subdivision is applied to mesh objects. To enable it, click the *Node view settings* button (it has an icon that looks like an eye), which is located in the top left of the *Mesh* tab in the toolbox (see previous tutorials in order to create a mesh). This will bring up the *Node view settings* popup. There you will see the live subdivision settings.

The *enable* check turns live subdivision on and off. The *iterations* entry controls the number of subdivision levels. The *max faces* entry limits the subdivision such that it will not subdivide beyond this number of faces; this prevents the program from freezing due to subdividing too far if the number of iterations was set too high by accident. The *normal sharpness* radio buttons controls how normal sharpness is preserved. The *wireframe mode* controls which subdivided edges are displayed.


# Keyboard shortcuts

Please note that all shortcuts can be edited using the *shortcuts editor*, found in the *Edit* menu.

### General - always available
|     |     |
| --- | --- |
| &lt;Ctrl&gt;-W | Switch between quad-viewport and single-viewport layouts |
| &lt;Ctrl&gt;-F | Focus tool |
| &lt;Ctrl&gt;-D | Drawing plane (toggle on/off) |
| &lt;Ctrl&gt;-&lt;Shift&gt;-S | Show scene graph window |
| &lt;Ctrl&gt;-Space | Choose model |

### Tabbed viewport layout (available when in tabbed viewport layout)

|     |     |
| --- | --- |
| &lt;Alt&gt;-Left arrow | Previous viewport |
| &lt;Alt&gt;-Right arrow | Next viewport |
| &lt;Alt&gt;-&lt;Shift&gt;-Left button | Previous viewport |
| &lt;Alt&gt;-&lt;Shift&gt;-Right button | Next viewport |


### Plan (always available)

|     |     |
| --- | --- |
| &lt;Control&gt;-Delete | Remove action from plan |
| &lt;Control&gt;-M | Modify action |


### All tools (available in all tools)

|     |     |
| --- | --- |
| \   | Dismiss tool; equivalent of clicking the _Done_ button. |



### Blank model / primitive  (available when editing a blank model; when a new scene node has been created) 

|     |     |
| --- | --- |
|     |
| B   | Box mesh |
| P   | Plane mesh |
| S   | Sphere mesh |
| C   | Cylinder mesh |
| &lt;Shift&gt;-C | Cone mesh |
| T   | Torus mesh |
| &lt;Shift&gt;-T | Tube mesh |
| F   | Draw face |


### Mesh  (available when editing a mesh)

|     |     |
| --- | --- |
| &lt;Alt&gt;-V | Vertex mode |
| &lt;Alt&gt;-E | Edge mode |
| &lt;Alt&gt;-F | Face mode |
| &lt;Alt&gt;-B | Body mode |



### Mesh editing - vertex mode (available when editing a mesh, in vertex mode)

|     |     |
| --- | --- |
| Space | Unselect all |
| &lt;Shift&gt;-Space | Select all |
| \[  | Shrink selection |
| \]  | Grow selection |
| W   | Move |
| E   | Scale |
| R   | Rotate |
| &lt;Shift&gt;-W | Move - normal |
| Q   | Tweak |
| &lt;Shift&gt;-Q | Surface-tweak |
| &lt;Shift&gt;-A | Template tweak |
| A   | Proportional tweak |
| F   | Flatten |
| X   | Expand |
| B   | Bevel |
| C   | Connect |
| T   | Cut |
| &lt;Shift&gt;-T | N-Cut |
| D   | Target weld |
| &lt;Shift&gt;-D | Weld |



### Mesh editing - edge mode (available when editing a mesh, in edge mode)

|     |     |
| --- | --- |
| Space | Unselect all |
| &lt;Shift&gt;-Space | Select all |
| \[  | Shrink selection |
| \]  | Grow selection |
| &lt;Shift&gt;-G | Pick edge ring path |
| &lt;Shift&gt;-L | Pick edge loop path |
| W   | Move |
| E   | Scale |
| R   | Rotate |
| &lt;Shift&gt;-W | Move - normal |
| Q   | Tweak |
| &lt;Shift&gt;-Q | Surface-tweak |
| A   | Proportional tweak |
| &lt;Shift&gt;-E | Slide |
| F   | Flatten |
| P   | Spin |
| I   | Rewire |
| X   | Extrude |
| &lt;Shift&gt;-X | Expand |
| B   | Bevel |
| C   | Connect |
| &lt;Shift&gt;-C | N-Connect |
| S   | Split |
| &lt;Shift&gt;-S | N-Split |
| T   | Cut |
| &lt;Shift&gt;-T | N-Cut |
| N   | Bandsaw |
| &lt;Shift&gt;-N | N-Bandsaw |
| V   | Dissolve |
| &lt;Shift&gt;-V | Clean dissolve |
| O   | Collapse |
| D   | Weld |



### Mesh editing - face mode  (available when editing a mesh, in face mode)

|     |     |
| --- | --- |
| Space | Unselect all |
| &lt;Shift&gt;-Space | Select all |
| \[  | Shrink selection |
| \]  | Grow selection |
| W   | Move |
| E   | Scale |
| R   | Rotate |
| &lt;Shift&gt;-W | Move - normal |
| Q   | Tweak |
| &lt;Shift&gt;-Q | Surface-tweak |
| A   | Proportional tweak |
| F   | Flatten |
| X   | Extrude |
| &lt;Shift&gt;-X | Free extrude |
| I   | Inset |
| &lt;Shift&gt;-I | Inset, with 'flow through marked edges' enabled |
| B   | Bevel |
| &lt;Shift&gt;-B | Bevel, with 'flow through marked edges' enabled |
| T   | Cut |
| &lt;Shift&gt;-T | N-Cut |
| D   | Weld |
| V   | Dissolve |
| O   | Collapse |
| Delete | Delete |
| &lt;Shift&gt;-D | Duplicate |


### Mesh editing - body mode (available when editing a mesh, in body mode)

|     |     |
| --- | --- |
| S   | Subdivide |
| K   | Knife |
| F   | Draw face |
| M   | Draw quad mesh |
| W   | Move |
| E   | Scale |
| R   | Rotate |
| H   | Shear |
| T   | Linear twist |
| &lt;Shift&gt;-T | Radial twist |
| P   | Taper |
| I   | Shrink |
| D   | Dent |
| B   | Bend |












