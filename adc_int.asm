;prekidna rutina za ADC
;
;#pragma vector=ADC12_VECTOR
;__interrupt void ADC12_ISR(void)
;{
;	if(ADC12IV == 6) // Vector  6: ADC12IFG0
;	{
;		adc1val = ADC12MEM0;
;	}
;	if(ADC12IV == 8) // Vector  8: ADC12IFG1
;	{
;		adc2val = ADC12MEM1;
;	}
;}
;
; Prekidna rutina ocitava vrednosti AD konvertora
; koji predstavlja trenutnu poziciju potenciometara.
			.cdecls C,LIST,"msp430.h"

			.ref adc1val			;promenljive iz main.c
			.ref adc2val

			.text

ADC12_ISR	cmp #6,ADC12IV
			jnz ELSE
			mov ADC12MEM0,adc1val
;			jmp END
ELSE		cmp #8,ADC12IV
			jnz END
			mov ADC12MEM1,adc2val
END			reti

			.sect  .int55
			.short ADC12_ISR

			.end
