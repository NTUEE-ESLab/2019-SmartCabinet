from __future__ import division
from visual import *
from time import clock

scene.width = 600
scene.height = 700
scene.background = color.white

mfrm = frame(axis=(1,0,0))                  # Motor Frame
rfrm = frame(frame=mfrm)                    # Rotor Frame

#base =
#floor = cylinder(length=4, height=0.5, width=4, color=color.blue)
#wall = rectangle(width=5, height=3) - rectangle(width=4, height=2)

ball = sphere(pos=(0,0,0), color=color.red, radius=0.1)
ball = sphere(pos=(0,4,0), color=color.red, radius=0.1)
#ball.velocity = vector(0,-1,0)

#	g1 = shapes.circle(radius=1.2)
#	ns=4
#	# We will have 24 contactor surfaces, 2 per each rotor wiring
#	for i in range(ns):
#	    # Second, subtract rectangular pieces to get a slice for each contact surface
#	    t = shapes.rectangle(pos=(1.2*cos(i*2*pi/ns),1.2*sin(i*2*pi/ns)),
#	                         width=2.1, height=0.05, rotate=i*2*pi/ns)
#	    g1 = g1 - t
#	
#	g1 = g1 - shapes.circle(radius=0.5)     # Last, subtract rotor shaft
#	cl = 2.0
#	# Now, extrude to get "cylindrical" contactor surfaces
#	ge1 = extrusion(pos=[(0,0,0),(0,cl,0)], shape=g1, color=(1,0.5,0.3),
#	                material=materials.rough, frame=rfrm)

#dt = 0.01
#while 1:
#    rate(100)
#    ball.pos = ball.pos + ball.velocity*dt
#    if ball.y < 1:
#        ball.velocity.y = -ball.velocity.y
#    else:
#        ball.velocity.y = ball.velocity.y - 9.8*dt

##Shell
#s = shapes.rectangle(pos=(0,8,0), width=1, height=17)
#se = extrusion(pos=paths.circle(radius=13), shape=s, material=materials.ice)

## Base
b1 = shapes.rectangle(pos=(0,-0.5,0), width=13, height=1)
b1e = extrusion(pos=paths.arc(angle1=0, angle2=3*pi/2, radius=8, np=100), shape=b1, material=materials.wood)
b2 = shapes.rectangle(pos=(0,8.5,0), width=13, height=1)
b2e = extrusion(pos=paths.arc(angle1=0, angle2=3*pi/2, radius=8, np=100), shape=b2, material=materials.wood)
b3 = shapes.rectangle(pos=(0,17,0), width=13, height=1)
b3e = extrusion(pos=paths.arc(angle1=0, angle2=3*pi/2, radius=8, np=100), shape=b3, material=materials.wood)

## Containers
mfrm = frame(axis=(1,0,0))                  # Motor Frame
f1frm = frame(frame=mfrm)                   # F1 Frame
f2frm = frame(frame=mfrm)                   # F2 Frame

f1 = shapes.rectangle(pos=(0,3.5,0), width=9, height=7) #- shapes.rectangle(pos=(0,2.5,0), width=6.5, height=4.5)
f2 = shapes.rectangle(pos=(0,12.5,0), width=9, height=7) #- shapes.rectangle(pos=(0,9.5,0), width=6.5, height=4.5)
gap = pi/30
f1_1 = extrusion(pos=paths.arc(angle1=0   +gap, angle2=pi/2 -gap, radius=7, np=100), shape=f1, material=materials.ice, frame=f1frm)
f1_2 = extrusion(pos=paths.arc(angle1=pi/2+gap, angle2=pi   -gap, radius=7, np=100), shape=f1, material=materials.ice, frame=f1frm)
f1_3 = extrusion(pos=paths.arc(angle1=pi  +gap, angle2=3*pi/2-gap, radius=7, np=100), shape=f1, material=materials.ice, frame=f1frm)
f2_1 = extrusion(pos=paths.arc(angle1=0   +gap, angle2=pi/2 -gap, radius=7, np=100), shape=f2, material=materials.ice, frame=f2frm)
f2_2 = extrusion(pos=paths.arc(angle1=pi/2+gap, angle2=pi   -gap, radius=7, np=100), shape=f2, material=materials.ice, frame=f2frm)
f2_3 = extrusion(pos=paths.arc(angle1=pi  +gap, angle2=3*pi/2-gap, radius=7, np=100), shape=f2, material=materials.ice, frame=f2frm)

##Lift
lffrm = frame(frame=mfrm)                   # Lift Frame
h = 23
lift = shapes.rectangle(pos=(0,h,0), width=9, height=7)
lifte = extrusion(pos=paths.arc(angle1=3*pi/2+gap, angle2=2*pi-gap, radius=7, np=100), shape=lift, material=materials.ice, frame=lffrm,color=color.blue)
lfb = shapes.rectangle(pos=(0,h-4,0), width=13, height=1)
lfbe = extrusion(pos=paths.arc(angle1=3*pi/2+gap/2, angle2=2*pi-gap/2, radius=8, np=100), shape=lfb, material=materials.wood)
## Wall



angl = pi/400
run = True
rotate = False
counter = 0
# Turn on the motor
while True:
    rate(100)
    if run:
    	if rotate == True:
    		f1frm.rotate(angle=angl, axis=(0,1,0))
    		counter +=1
    	else:
    		lifte.visible = 0
        	lfbe.visible = 0
        	del lifte
        	del lfbe
        	h = h - 0.02
        	print(h)
        	if h <= 3.5:
    			rotate = True
		lift = shapes.rectangle(pos=(0,h,0), width=9, height=7)
		lifte = extrusion(pos=paths.arc(angle1=3*pi/2+gap, angle2=2*pi-gap, radius=7, np=100), shape=lift, material=materials.ice, frame=f1frm, color=color.blue)
		lfb = shapes.rectangle(pos=(0,h-4,0), width=13, height=1)
		lfbe = extrusion(pos=paths.arc(angle1=3*pi/2+gap/2, angle2=2*pi-gap/2, radius=8, np=100), shape=lfb, material=materials.wood)
    if counter == 600:
    	counter = 0
    	run = not run
    if scene.mouse.events:
        m = scene.mouse.getevent()
        if m.click == 'left':
            run = not run
