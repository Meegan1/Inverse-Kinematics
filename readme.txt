This project uses the NuGet package manager in visual studio to include opengl and glm, ensure that these are imported!


Usage:
    Run the project, GUI controls are given to edit the animation.

    Joint List - Select a joint to be edited
    Play/Pause - play and pauses the current animation
    Record - Records the viewport at 60fps, outputting the frames into the video folder, allowing for ffmpeg to be used to render the images to a video
    Edit Joint - Edits the currently selected joint (Right click + drag to move the ball)
    Save - Saves current animation into bvh file
    Load - Loads an animation into the viewport

    Pose Mixing Section:
        +/- - add/remove a pose from the list
        Generate Animation - Generates an animation based on the interpolation between all poses
        
    Note: the order of the list indicates the order of interpolation, this can be dragged to re-order

Editing a Joint: 
    - Select Joint
    - Click Edit Joint
    - Ball will turn red (indicating the joint is being edited)
    - Right click and drag to move the ball, it moves relative to the camera position.
    - Save bvh file with new pose

Generating animation from interpolation of poses:
    - Click + to add poses to the list
    - Re-order via dragging the poses to the desired order
    - Click Generate Animation to output the animation into a bvh file

Recording:
    Recording can be done by clicking record, the frames are then outputted into the video folder, along with the ffmpeg command needed to 
    render the frames into a video.


Controls:
    WASD - camera movement
    Left Click - rotate camera
    Right Click - edit joint (when edit joint is enabled)