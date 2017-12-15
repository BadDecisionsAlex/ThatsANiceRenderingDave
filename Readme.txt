
============
Controls
============
1. Fluid System
2. Space System
3. Colorful Space System
4. Collision and Gravity system
5. Smoke System
6. Emitted Smoke (Fire) System
7. Grid System
8. Pixel Grid System

-Smoke System-

    Mouse:
	Left Click: Push particles
	Right Click: Pull particles


-Fluid System- 

    This solution is based on Navier Stokes Equations and that paper by Jos Stam

    Mouse:
        Left Click : Apply Force
        Right Click : add/remove density

    Keys:
        R : changes active color to red 
        G : changes active color to green
        B : changes active color to blue

        CTRL : swaps between adding or removing dye
        
        C : clears screen
        \ : displays current settings

        - : reduces diffusion
        = : increases diffusion
        [ : reduces viscocity
        ] : increases viscocity
        ; : reduces force
        ' : increases force
        . : reduces amount of dye added or removed
        / : increases amount of dye added or removed

        a : adds an emitter at the mouse position with the current dye settings
        UP : adds Vertex emitter pointed up
        DOWN : adds Vertex emitter pointed down
        LEFT : adds Vertex emitter pointed left
        RIGHT : adds Vertex emitter pointed right
