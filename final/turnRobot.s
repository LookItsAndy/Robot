.cpu cortex-a53
.fpu neon-fp-armv8
.data
.text

left: .asciz "Turning Left"
slightLeft: .asciz "Turning slight left"
center: .asciz "Going center"
slightRight: .asciz "Turning slight right"
right: .asciz "Turning right"
stop: .asciz "Stop"
.global leftTurnfactor
.global rightTurnFactor
.align 2
.global turnRobot
.type turnRobot, %function

turnRobot:
@r0 = direction
	push {fp, lr}
	add fp, sp, #4
	push {r5} @fp-8
	push {r6} @fp-12
	push {r7} @fp-16
	push {r8} @fp-20
	push {r10} @fp-24, direction
	push {r11} @ for multiplication
	
	mov r5, #0
	mov r6, #0
	mov r6, #0
	mov r7, #0
	mov r8, #0

	ldr r7, =leftTurnFactor
	ldr r8, =rightTurnFactor @address
	mov r10, r0 @ r10 now had direction

	if_left:
		cmp r10, #0
		bne if_slightLeft
		
		mov r5, #100
		mov r11, #75
		div r5, r11, r5

		mov r5, #0.75
		mov r6, #1

		ldr r0, =left
		bl printf

		b if_end
	if_slightLeft:
		cmp r10, #1
		bne if_center

		mov r5, #100
		mov r11, #90
		div r5, r11, r5

		mov r5, #0.9
		mov r6, #1

		ldr r0, =slightLeft
		bl printf

		b if_end
	if_center:
		cmp r10, #2
		bne if_slightRight
		mov r5, #1
		mov r6, #1

		ldr r0, =center
		bl printf

		b if_end
	if_slightRight:
		cmp r10, #3
		bne if_fullRight
		mov r5, #1

		mov r6, #100
		mov r11, #90
		div r6, r11, r6

		ldr r0, =slightRight
		bl printf

		mov r6, #0.9
		b if_end
	if_fullRight:
		cmp r10, #4
		bne if_nothing
		mov r5, #1

		mov r6, #100
		mov r11, #75
		div r6, r11, r6

		ldr r0, =right
		bl printf

		mov r6, #0.75
		b if_end
	if_nothing:
		mov r5, #0
		mov r6, #0

		ldr r0, =stop
		bl stop

		b if_end
	if_end:
		str r7, [r5] @store value into leftTurnFactor
		str r8, [r6] @ store value into rightTurnFactor

		ldr r5, [fp,#-8]
		ldr r6, [fp, #-12]
		ldr r7, [fp, #-16]
		ldr r8, [fp, #-20]
		ldr r10, [fp, #-24]
		sub sp, fp, #4
		pop {fp, pc}
	
	