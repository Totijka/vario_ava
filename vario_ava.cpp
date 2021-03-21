#include "vario_ava.h"

boolean defFlag0, defFlag2, mapFlag0, mapFlag1, mapFlag2;

void PIN_set(uint8_t pin, uint8_t x) {
	if (pin < 8) bitWrite(PORTD, pin, x);
	else if (pin < 14) bitWrite(PORTB, (pin - 8), x);
	else if (pin < 20) bitWrite(PORTC, (pin - 14), x);
}

/* биты делителя называются CSn2/CSn1/CSn0 в регистре TCCRnB  где n- номер таймера*/
void PWM_default(byte pin) { // тут настроен максимально дефолтные настройки таймера, делитель 64
	switch (pin) {
	case 3: //Timer2_B  // не хочешь /64 можешь сделать /1
		defFlag2 = 1; // врубаем флаг дефолт для ноги 3
		mapFlag2 = 0; //выкл масштабирование
		TCCR2A = 0b10100001;  //default pwm 8 bit phaseCorrect
		TCCR2B = 0b00000100;  // prescaler /64  // крайние левые 3 бита поменять на 001 для делителя 1
		break;
	case 5: //Timer0_B
		defFlag0 = 1; // врубаем флаг дефолт для ноги 5
		mapFlag0 = 0;
		TCCR0A = 0b10100011; //default pwm 8 bit FastPWM
		TCCR0B = 0b00000011; // prescaler /64
		break;
	case 6: //Timer0_A
		defFlag0 = 1; // врубаем флаг дефолт для ноги 5
		mapFlag0 = 0;
		TCCR0A = 0b10100011; //default pwm 8 bit FastPWM
		TCCR0B = 0b00000011; // prescaler /64
		break;
	case 9: //Timer1_A
		mapFlag1 = 0;
		TCCR1A = 0b10100001; // default pwm 8 bit phaseCorrect
		TCCR1B = 0b00000011; // prescaler /64 // крайние левые 3 бита в 001 для делителя 1
		break;
	case 10: //Timer1_B
		mapFlag1 = 0;
		TCCR1A = 0b10100001; // default pwm 8 bit phaseCorrect
		TCCR1B = 0b00000011; // prescaler /64
		break;
	case 11: //Timer2_A
		defFlag0 = 1; // врубаем флаг дефолт для ноги 5
		mapFlag0 = 0;
		TCCR2A = 0b10100011; //default pwm 8 bit FastPWM
		TCCR2B = 0b00000011; // prescaler /64
		break;
	}
}

void PWM_frequency(byte pin, long freq, uint8_t correct) {
	byte top0 , top2;
	unsigned int top1;
	freq = constrain(freq, 50, 200000);
	switch (pin) {
	case 3:
		mapFlag2 = 1; // вкл масштабирование
		defFlag2 = 0; // откл дефолтность
		switch (correct) {
		case 1: //T2_corr
			TCCR2A = 0b00100001; // тут настраиваем коррект (2 с права бит в 0)
			if (freq > 31373) {  //разбиваем на диапазоны частот
				TCCR2B = 0b00001001;   //1 подбираем делители
				top2 = 8000000 / freq;  //  формула для расчета предела счета в режиме короректа
			}
			if (freq <= 31373 && freq > 3922) {
				TCCR2B = 0b00001010;   //8
				top2 = 1000000 / freq; // для каждого делителя свои расчеты
			}
			if (freq <= 3922 && freq > 981) {
				TCCR2B = 0b00001011;   //32
				top2 = 250000 / freq;
			}
			if (freq <= 981 && freq > 491) {
				TCCR2B = 0b00001100;   //64
				top2 = 125000 / freq;
			}
			if (freq <= 491 && freq > 246) {
				TCCR2B = 0b00001101;   //128
				top2 = 62500 / freq;
			}
			OCR2A = top2; // в конце подставили предел счета
			break;
		case 0: //T2_fast  // тут настраивается фаст
			TCCR2A = 0b00100011;
			if (freq > 62500) {
				TCCR2B = 0b00001001;   //1
				top2 = (16000000 / freq) - 1; // формула для режима фаста
			}
			if (freq <= 62500 && freq > 7813) {
				TCCR2B = 0b00001010;   //8
				top2 = (2000000 / freq) - 1;
			}
			if (freq <= 7813 && freq > 1954) {
				TCCR2B = 0b00001011;   //32
				top2 = (500000 / freq) - 1;
			}
			if (freq <= 1954 && freq > 977) {
				TCCR2B = 0b00001100;   //64
				top2 = (250000 / freq) - 1;
			}
			if (freq <= 977 && freq > 489) {
				TCCR2B = 0b00001101;   //128
				top2 = (125000 / freq) - 1;
			}
			if (freq <= 489) {
				TCCR2B = 0b00001110;   //256
				top2 = (62500 / freq) - 1;
			}
			OCR2A = top2;
			break;
		}
		break;
	case 5:
		mapFlag0 = 1;
		defFlag0 = 0;
		switch (correct) {
		case 1: //T0_corr
			TCCR0A = 0b00100001;
			if (freq > 31373) {
				TCCR0B = 0b00001001;   //1
				top0 = 8000000 / freq;
			}
			if (freq <= 31373 && freq > 3922) {
				TCCR0B = 0b00001010;   //8
				top0 = 1000000 / freq;
			}
			if (freq <= 3922 && freq > 491) {
				TCCR0B = 0b00001011;   //64
				top0 = 125000 / freq;
			}
			if (freq <= 491) {
				TCCR0B = 0b00001100;   //256
				top0 = 31250 / freq;
			}
			OCR0A = top0;
			break;
		case 0: //T0_fast
			TCCR0A = 0b00100011;
			if (freq > 62500) {
				TCCR0B = 0b00001001;   //1
				top0 = (16000000 / freq) - 1;
			}
			if (freq <= 62500 && freq > 7813) {
				TCCR0B = 0b00001010;   //8
				top0 = (2000000 / freq) - 1;
			}
			if (freq <= 7813 && freq > 977) {
				TCCR0B = 0b00001011;   //64
				top0 = (250000 / freq) - 1;
			}
			if (freq <= 977) {
				TCCR0B = 0b00001100;   //256
				top0 = (62500 / freq) - 1;
			}
			OCR0A = top0;
			break;
		}
		break;
	case 9: // тут таймер 1  и ему хорошо
		mapFlag1 = 1;
		switch (correct) {
		case 1: //T1_corr
			TCCR1A = 0b10100010;  // сразу настраиваем коррект или нет
			TCCR1B = (freq > 256)? 0b00010001 : 0b00010010; // сразу ставим делитель 1
			if (CLKPR & (1 << CLKPS0)) {
				top1 = (freq > 256)? 8000000 / (freq*2) : 1000000 / (freq*2); // тупо считаем топ и подставляем
			}
			else {
				top1 = (freq > 256)? 8000000 / freq : 1000000 / freq; // тупо считаем топ и подставляем
			}
			ICR1H = highByte(top1);
			ICR1L = lowByte(top1);
			break;
		case 0: //T1_fast
			TCCR1A = 0b10100010;  // аналогично для режима фаст
			TCCR1B = 0b00011001;
			top1 = (16000000 / freq) - 1;
			ICR1H = highByte(top1);
			ICR1L = lowByte(top1);
			break;
		}
		break;
	case 10:
		mapFlag1 = 1;
		switch (correct) {
		case 1: //T1_corr
			TCCR1A = 0b10100010;
			TCCR1B = 0b00010001;
			top1 = 8000000 / freq;
			ICR1H = highByte(top1);
			ICR1L = lowByte(top1);
			break;
		case 0: //T1_fast
			TCCR1A = 0b10100010;
			TCCR1B = 0b00011001;
			top1 = (16000000 / freq) - 1;
			ICR1H = highByte(top1);
			ICR1L = lowByte(top1);
			break;
		}
		break;
	}
}

void PWM_resolution(byte pin, byte res, uint8_t correct) {
	res = constrain(res, 4, 16); // предел счета это кол-во ступеней (2 в степени)
	unsigned int top = pow(2, res) - 1;
	switch (pin) {
	case 3: //Timer2_B
		defFlag2 = 0; // выключаем флаг дефолтности для таймера 2
		mapFlag2 = 0; // выключаем флаг мастабирования
		if (correct) {
			TCCR2A = 0b10100001; // настроили как коррект
			TCCR2B = 0b00001001; // вкл делитель 1
			OCR2A = constrain(top, 0, 255); // ограничили значение до 8 бит
		}
		else {
			TCCR2A = 0b10100011; //настроили как фаст
			TCCR2B = 0b00001001; // вкл делитель 1
			OCR2A = constrain(top, 0, 255);
		}
		break;
	case 5: //Timer0_B
		defFlag0 = 0;
		mapFlag0 = 0;
		if (correct) {
			TCCR0A = 0b10100001;
			TCCR0B = 0b00001001;
			OCR0A = constrain(top, 0, 255);
		}
		else {
			TCCR0A = 0b10100011;  // аналогично
			TCCR0B = 0b00001001;
			OCR0A = constrain(top, 0, 255);
		}
		break;
	case 9: //Timer1_A
		mapFlag1 = 0;
		if (correct) {
			TCCR1A = 0b10100010;
			TCCR1B = 0b00010001;
			ICR1H = highByte(top); // раскидалии топ по 2м битам
			ICR1L = lowByte(top);
		}
		else {
			TCCR1A = 0b10100010;
			TCCR1B = 0b00011001;
			ICR1H = highByte(top);
			ICR1L = lowByte(top);
		}
		break;
	case 10: //Timer1_B
		mapFlag1 = 0;
		if (correct) {
			TCCR1A = 0b10100010;
			TCCR1B = 0b00010001;
			ICR1H = highByte(top);
			ICR1L = lowByte(top);
		}
		else {
			TCCR1A = 0b10100010;
			TCCR1B = 0b00011001;
			ICR1H = highByte(top);
			ICR1L = lowByte(top);
		}
		break;
	}
}

void PWM_set(byte pin , unsigned int duty) { // имеем баг с таймерами в режиме fastPWM с прямым шим и исправляем его супер образом
	cli(); //запрет прерываний
	switch (pin) {  // баг заключается в то, что при заполнении 0 на выходе заполнение около 1-5%
	case 3: //Timer2_B
		if (mapFlag2) {
			duty = map(duty, 0, 255, 0, OCR2A); // от 0 до top который пизданули с регистра
		}
		if (duty == 0) {
			TCCR2A &= ~(1 << COM2B1); //отрубаем регистр сравнения
			bitClear(PORTD, 3); // выкл
		} else if (duty == OCR2A) {
			TCCR2A &= ~(1 << COM2B1); //отрубаем регистр сравнения
			bitSet(PORTD, 3); // вкл
		} else {
			TCCR2A |= (1 << COM2B1); // подрубаем к пину регистр сравнения
			OCR2B = duty; // настроили скважность
		}
		break;
	case 5: //Timer0_B
		if (mapFlag0) {
			duty = map(duty, 0, 255, 0, OCR0A);
		}
		if (duty == 0) {
			TCCR0A &= ~(1 << COM0B1);
			bitClear(PORTD, 5);
		} else if (duty == OCR0A) {
			TCCR0A &= ~(1 << COM0B1);
			bitSet(PORTD, 5);
		} else {
			TCCR0A |= (1 << COM0B1);
			OCR0B = duty;
		}
		break;
	case 6: //Timer0_A
		if (defFlag0) {
			if (duty == 0) {
				TCCR0A &= ~(1 << COM0B1);
				bitClear(PORTD, 6);
			} else if (duty == 255) {
				TCCR0A &= ~(1 << COM0B1);
				bitSet(PORTD, 6);
			} else {
				TCCR0A |= (1 << COM0B1);
				OCR0A = duty;
			}
		} else {
			bitSet(TCCR0A, COM0A1);
			OCR0A = duty;
		}
		break;
	case 9: //Timer1_A
		if (mapFlag1) {
			if (CLKPR & (1 << CLKPS0)) {
			duty = (TCCR1B == 0b00010001)? duty/2 : duty/(2*8);//map(duty, 0, 255, 0, ICR1L + (ICR1H << 8));
		    }
		    else {
		    duty = (TCCR1B == 0b00010001)? duty : duty/8;//map(duty, 0, 255, 0, ICR1L + (ICR1H << 8));	
		    }
		}
		if (duty == 0) {
			TCCR1A &= ~(1 << COM1A1);
			bitClear(PORTB, 1);
		} else if (duty == (ICR1L + (ICR1H << 8)) ) {
			TCCR1A &= ~(1 << COM1A1);
			bitSet(PORTB, 1);
		} else {
			TCCR1A |= (1 << COM1A1);
			OCR1AH = highByte(duty);
			OCR1AL = lowByte(duty);
		}
		break;
	case 10: //Timer1_B
		if (mapFlag1) {
			duty = map(duty, 0, 255, 0, ICR1L + (ICR1H << 8));
		}
		if (duty == 0) {
			TCCR1A &= ~(1 << COM1B1);
			bitClear(PORTB, 2);
		} else if (duty == (ICR1L + (ICR1H << 8)) ) {
			TCCR1A &= ~(1 << COM1B1);
			bitSet(PORTB, 2);
		} else {
			TCCR1A |= (1 << COM1B1);
			OCR1BH = highByte(duty);
			OCR1BL = lowByte(duty);
		}
		break;
	case 11: //Timer2_A // аналогично, будет работать только с функцией pwm_default(11);
		if (defFlag2) {
			if (duty == 0) {
				TCCR2A &= ~(1 << COM2A1);
				bitClear(PORTB, 3);
			} else if (duty == 255) {
				TCCR2A &= ~(1 << COM2A1);
				bitSet(PORTB, 3);
			} else {
				TCCR2A |= (1 << COM2A1);
				OCR2A = duty;
			}
		} else {
			bitSet(TCCR2A, COM2A1);
			OCR2A = duty;
		}
		break;
	}
	sei();
}

void PWM_detach(byte pin) {
	switch (pin) {
	case 3: TCCR2A &= ~(1 << COM2B1);
		PIN_set(3, 0);
		break;
	case 5: TCCR0A &= ~(1 << COM0B1);
		PIN_set(5, 0);
		break;
	case 6: TCCR0A &= ~(1 << COM0B1);
		PIN_set(6, 0);
		break;
	case 9: TCCR1A &= ~(1 << COM1A1);
		PIN_set(9, 0);
		break;
	case 10: TCCR1A &= ~(1 << COM1B1);
		PIN_set(10, 0);
		break;
	case 11: TCCR2A &= ~(1 << COM2A1);
		PIN_set(11, 0);
		break;
	}
}

void PWM_attach(byte pin) {
	switch (pin) {
	case 3: TCCR2A |= (1 << COM2B1);
		break;
	case 5: TCCR0A |= (1 << COM0B1);
		break;
	case 6: TCCR0A |= (1 << COM0B1);
		break;
	case 9: TCCR1A |= (1 << COM1A1);
		break;
	case 10: TCCR1A |= (1 << COM1B1);
		break;
	case 11: TCCR2A |= (1 << COM2A1);
		break;
	}
}

void PWM_prescaler(byte pin, byte mode) {
	byte prescale;
	if (pin == 5 || pin == 6) {
		switch (mode) {
		case 1: prescale = 0x01; break;
		case 2: prescale = 0x02; break;
		case 3: prescale = 0x03; break;
		case 4: prescale = 0x04; break;
		case 5: prescale = 0x05; break;
		default: return;
		}
	} else if (pin == 9 || pin == 10) {
		switch (mode) {
		case 1: prescale = 0x01; break;
		case 2: prescale = 0x02; break;
		case 3: prescale = 0x03; break;
		case 4: prescale = 0x04; break;
		case 5: prescale = 0x05; break;
		default: return;
		}
	} else if (pin == 3 || pin == 11) {
		switch (mode) {
		case 1: prescale = 0x01; break;
		case 2: prescale = 0x02; break;
		case 3: prescale = 0x03; break;
		case 4: prescale = 0x04; break;
		case 5: prescale = 0x05; break;
		case 6: prescale = 0x06; break;
		case 7: prescale = 0x07; break;
		default: return;
		}
	}
	if (pin == 5 || pin == 6) {
		TCCR0B = 0;
		TCCR0B = TCCR0B & 0b11111000 | prescale;
	} else if (pin == 9 || pin == 10) {
		TCCR1B = 0;
		TCCR1B = TCCR1B & 0b11111000 | prescale;
	} else if (pin == 3 || pin == 11) {
		TCCR2B = 0;
		TCCR2B = TCCR2B & 0b11111000 | prescale;
	}
}

void PWM_mode(byte pin, uint8_t mode) {		// 0 - FastPWM, 1 - Phase-correct PWM
	if (pin == 5 || pin == 6) {
		if (mode) TCCR0A |= _BV(WGM00);
		else TCCR0A |= _BV(WGM00) | _BV(WGM01);
	} else if (pin == 9 || pin == 10) {
		if (mode) TCCR1B &= ~_BV(WGM12);
		else TCCR1B |= _BV(WGM12);
	} else if (pin == 3 || pin == 11) {
		if (mode) TCCR2A |= _BV(WGM20);
		else TCCR2A |= _BV(WGM20) | _BV(WGM21);
	} else {
		return;
	}
}

void PWM_TMR1_8BIT() {
	TCCR1A = TCCR1A & 0xe0 | 1;
}
void PWM_TMR1_10BIT() {
	TCCR1A = TCCR1A & 0xe0 | 3;
}

void PWM_16KHZ_D3(byte duty) {
	TCCR2A = 0b10100011;
	TCCR2B = 0b00001010;
	OCR2A = 124;
	if (duty == 0) {
		bitClear(TCCR2A, COM2B1);
		//PIN_set(3, 0);
		bitClear(PORTD, 3);
	} else {
		bitSet(TCCR2A, COM2B1);
		OCR2B = map(duty, 0, 255, 0, 124);
	}
}

void PWM_20KHZ_D3(byte duty) {
	TCCR2A = 0b10100011;
	TCCR2B = 0b00001010;
	OCR2A = 99;
	if (duty == 0) {
		bitClear(TCCR2A, COM2B1);
		//PIN_set(3, 0);
		bitClear(PORTD, 3);
	} else {
		bitSet(TCCR2A, COM2B1);
		OCR2B = map(duty, 0, 255, 0, 99);
	}
}

void PWM_16KHZ_D5(byte duty) {
	TCCR0A = 0b10100011;
	TCCR0B = 0b00001010;
	OCR0A = 124;
	if (duty == 0) {
		bitClear(TCCR0A, COM0B1);
		//PIN_set(5, 0);
		bitClear(PORTD, 5);
	} else {
		bitSet(TCCR0A, COM0B1);
		OCR0B = map(duty, 0, 255, 0, 124);
	}
}

void PWM_20KHZ_D5(byte duty) {
	TCCR0A = 0b10100011;
	TCCR0B = 0b00001010;
	OCR0A = 99;
	if (duty == 0) {
		bitClear(TCCR0A, COM0B1);
		//PIN_set(5, 0);
		bitClear(PORTD, 5);
	} else {
		bitSet(TCCR0A, COM0B1);
		OCR0B = map(duty, 0, 255, 0, 99);
	}
}

void PWM_16KHZ_D9(int duty) {
	TCCR1A = 0b10100010;
	TCCR1B = 0b00011001;
	ICR1H = 3;		// highByte(1023)
	ICR1L = 255;	// lowByte(1023)	
	OCR1AH = highByte(duty);
	OCR1AL = lowByte(duty);
}
void PWM_20KHZ_D9(int duty) {
	TCCR1A = 0b10100010;
	TCCR1B = 0b00011001;
	ICR1H = 3;		// highByte(799)
	ICR1L = 31;		// lowByte(799)	
	duty = map(duty, 0, 1023, 0, 799);
	OCR1AH = highByte(duty);
	OCR1AL = lowByte(duty);
}
void PWM_16KHZ_D10(int duty) {
	TCCR1A = 0b10100010;
	TCCR1B = 0b00011001;
	ICR1H = 3;		// highByte(1023)
	ICR1L = 255;	// lowByte(1023)
	OCR1BH = highByte(duty);
	OCR1BL = lowByte(duty);
}
void PWM_20KHZ_D10(int duty) {
	TCCR1A = 0b10100010;
	TCCR1B = 0b00011001;
	ICR1H = 3;		// highByte(799)
	ICR1L = 31;		// lowByte(799)
	duty = map(duty, 0, 1023, 0, 799);
	OCR1BH = highByte(duty);
	OCR1BL = lowByte(duty);
}

float PWM_square_D9(float frequency) {
	uint32_t top;
	float realF;
	TCCR1A = 0b01010000;
	TCCR1C = 0b01000000;
	if (frequency < 16) {
		TCCR1B = 0b00011011;
		top = (float)250000 / frequency / 2 - 1;
		realF = (float)250000 / 2 / (1 + top);
	} else if (frequency < 125) {
		TCCR1B = 0b00011010;
		top = (float)2000000 / frequency / 2 - 1;
		realF = (float)2000000 / 2 / (1 + top);
	} else {
		TCCR1B = 0b00011001;
		top = (float)16000000 / frequency / 2 - 1;
		realF = (float)16000000 / 2 / (1 + top);
	}
	ICR1H = highByte(top);
	ICR1L = lowByte(top);
	return realF;
}

/* **************************** my funcs ********************************** */

void buzz_end_of_flight(int buzz_vol)
{
  freq_shift_off();
  PWM_set(9, buzz_vol);
  for (int i = 0; i < 50; i++) {
    PWM_frequency(9, 700-i*8, CORRECT_PWM);//FAST_PWM);
    delay(10/DIV_FACTOR);
  }
  PWM_set(9, 0);
  freq_shift_on();
}

/* ********************************************************************** */

int buzz_size_array;
int buzz_up_0_thres;
int buzz_up_1_thres;
int buzz_up_2_thres;
int buzz_up_3_thres;
int buzz_down_0_thres;
unsigned int pwdown_time;
int buzz_up_start_freq;
int buzz_down_start_freq;
int buzz_up_factor;
int buzz_down_factor;
int battery_alarm_level;
int bat_temp_en;
int buzz_always;
int flight_total;
int flight_last;
int buzz_volume;
int flight_time;
unsigned int total_flight_time;
unsigned int flight_start_filter;
unsigned int flight_stop_filter;
int battery_calibration;
unsigned int working_time, total_working_time;

void update_int(int addr, int val)
{
  EEPROM.update(addr,highByte(val));
  EEPROM.update(addr+1,lowByte(val));  
}

void update_params()
{
  update_int(0*2,buzz_size_array);
  update_int(1*2,buzz_up_0_thres);
  update_int(2*2,buzz_up_1_thres);
  update_int(3*2,buzz_up_2_thres);
  update_int(4*2,buzz_up_3_thres);
  update_int(5*2,buzz_down_0_thres);
  update_int(6*2,pwdown_time);
  update_int(7*2,buzz_up_start_freq);
  update_int(8*2,buzz_down_start_freq);
  update_int(9*2,buzz_up_factor);
  update_int(10*2,buzz_down_factor);
  update_int(11*2,battery_alarm_level);
  update_int(12*2,bat_temp_en);
  update_int(13*2,buzz_always);
  update_int(14*2,buzz_volume);
  update_int(15*2,flight_start_filter);
  update_int(16*2,flight_time);
  update_int(17*2,total_flight_time);
  update_int(18*2,battery_calibration);
  update_int(19*2,total_working_time);
  update_int(20*2,flight_stop_filter);

}

int read_int(int addr)
{
return EEPROM.read(addr)*256 + EEPROM.read(addr+1);
}

void read_params()
{
  buzz_size_array = read_int(0*2);
  buzz_up_0_thres = read_int(1*2);
  buzz_up_1_thres = read_int(2*2);
  buzz_up_2_thres = read_int(3*2);
  buzz_up_3_thres = read_int(4*2);
  buzz_down_0_thres = read_int(5*2);
  pwdown_time = read_int(6*2);
  buzz_up_start_freq = read_int(7*2);
  buzz_down_start_freq = read_int(8*2);
  buzz_up_factor = read_int(9*2);
  buzz_down_factor = read_int(10*2);
  battery_alarm_level = read_int(11*2);
  bat_temp_en = read_int(12*2);
  buzz_always = read_int(13*2);
  buzz_volume = 0;//read_int(14*2);
  flight_start_filter = read_int(15*2);
  flight_time = !flight? (unsigned int) read_int(16*2) : flight_time;
  total_flight_time = !flight? (unsigned int) read_int(17*2) : total_flight_time;
  battery_calibration = read_int(18*2);
  total_working_time = (unsigned int) read_int(19*2);
  flight_stop_filter = read_int(20*2);
}

void default_params()
{
buzz_size_array = 20;
buzz_up_0_thres = 20;
buzz_up_1_thres = 50;
buzz_up_2_thres = 100;
buzz_up_3_thres = 300;
buzz_down_0_thres = -150;
pwdown_time = 60;
buzz_up_start_freq = 700;
buzz_down_start_freq = 300;
buzz_up_factor = 4;
buzz_down_factor = 8;
battery_alarm_level = 40;
bat_temp_en = 0;
buzz_always = 0;
buzz_volume = 50;
flight_start_filter = 4000;
flight_stop_filter = 60;
}

/* ************************************************************ */

float read_voltage()
{
  digitalWrite(battery_tr_pin,1);
  batt_average = 0.95 * batt_average + 0.05 * ((float) analogRead(battery_pin)) * 2 * 3.3 / 1024;
  digitalWrite(battery_tr_pin,0);

  return batt_average ;
}

// void wait(int del)
// {

//   unsigned long start_sleep = millis();

//   bool bb = 0;

// do {
// 	delay(10);
// } while (millis() < start_sleep + 10);

//   // while (millis() < start_sleep + 10) {
//     // delay(100);
//     // bb = 1;
//     // power.sleep(SLEEP_128MS);
//   // }
//     delay(100);

// }

//void press_button() {
//  button = 1;
//  button_time = millis();
//}