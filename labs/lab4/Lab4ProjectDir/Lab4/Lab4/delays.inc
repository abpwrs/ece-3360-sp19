// delays


_delay_100_ms:
	  push r23
	  push r24
	  push r25
	  nop
	  ldi r23, 20      ; r23 <-- Counter for outer loop
  d11: ldi r24, 44     ; r24 <-- Counter for level 2 loop
  d12: ldi r25, 227     ; r25 <-- Counter for inner loop
  d13: dec r25
      nop               ; no operation
      brne d13
      dec r24
      brne d12
      dec r23
      brne d11
	  nop
	  pop r25
	  pop r24
	  pop r23
      ret

_delay_10_ms:
	  push r23
	  push r24
	  push r25
	  nop
	  ldi r23, 2      ; r23 <-- Counter for outer loop
  d21: ldi r24, 44     ; r24 <-- Counter for level 2 loop
  d22: ldi r25, 227     ; r25 <-- Counter for inner loop
  d23: dec r25
      nop               ; no operation
      brne d23
      dec r24
      brne d22
      dec r23
      brne d21
	  nop
	  pop r25
	  pop r24
	  pop r23
      ret


_delay_200_us:
	  push r23
	  push r24
	  push r25
	  nop
	  ldi r23, 1       ; r23 <-- Counter for outer loop
  d31: ldi r24, 5       ; r24 <-- Counter for level 2 loop
  d32: ldi r25, 100     ; r25 <-- Counter for inner loop
  d33: dec r25
      nop               ; no operation
      brne d33
      dec r24
      brne d32
      dec r23
      brne d31
	  nop
	  pop r25
	  pop r24
	  pop r23
      ret


