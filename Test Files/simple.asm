    ; Store the numbers in memory
    ldc 6            ; Load the first number (6)
    stl 0            ; Store it at [SP + 0]
    ldc 9            ; Load the second number (9)
    stl 1            ; Store it at [SP + 1]
    ldc 4            ; Load the third number (4)
    stl 2            ; Store it at [SP + 2]
    ldc 9            ; Load the fourth number (9)
    stl 3            ; Store it at [SP + 3]
    ldc 2            ; Load the fifth number (2)
    stl 4            ; Store it at [SP + 4]

    ; Initialize loop variables
    ldc 5            ; Total count of numbers (5)
    stl 5            ; Store count in [SP + 5] (loop counter)

    ldc 0            ; Initialize the result to 0
    stl 6            ; Store result in [SP + 6]

    ldc 0            ; Initialize the base pointer (pointing to [SP + 0])
    stl 7            ; Store the base pointer in [SP + 7]

    ; Loop to sum numbers
    a:add_loop
    ldl 5            ; Load the loop counter
    brz end_add      ; If counter is zero, exit the loop

    ldl 7            ; Load the base pointer (current element address)
    ldnl 0          ; Load the value at the base pointer memory[A+offset]
    ldl 6            ; Load the current result
    add              ; Add the value to the result
    stl 6            ; Store the updated result in [SP + 6]

    ldl 7            ; Load the base pointer
    ldc 1            ; Load 1 (to move to the next element)
    add              ; Increment the base pointer
    stl 7            ; Store the updated base pointer in [SP + 7]

    ldl 5            ; Load the loop counter
    ldc -1           ; Decrement the loop counter
    add              ; Update the loop counter
    stl 5            ; Store the updated counter in [SP + 5]

    br add_loop      ; Repeat the loop

end_add:
    HALT             ; Stop the emulator
