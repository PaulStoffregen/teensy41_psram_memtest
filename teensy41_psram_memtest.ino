extern "C" uint8_t external_psram_size;

bool memory_ok = false;
uint32_t *memory_begin, *memory_end;

bool check_fixed_pattern(uint32_t pattern);
bool check_lfsr_pattern(uint32_t seed);

void setup()
{
	while (!Serial) ; // wait
	pinMode(13, OUTPUT);
	uint8_t size = external_psram_size;
	Serial.printf("EXTMEM Memory Test, %d Mbyte\n", size);
	if (size == 0) return;
	const float clocks[4] = {396.0f, 720.0f, 664.62f, 528.0f};
	const float frequency = clocks[(CCM_CBCMR >> 8) & 3] / (float)(((CCM_CBCMR >> 29) & 7) + 1);
	Serial.printf(" CCM_CBCMR=%08X (%.1f MHz)\n", CCM_CBCMR, frequency);
	memory_begin = (uint32_t *)(0x70000000);
	memory_end = (uint32_t *)(0x70000000 + size * 1048576);
	elapsedMillis msec = 0;
	if (!check_fixed_pattern(0x5A698421)) return;
	if (!check_lfsr_pattern(2976674124ul)) return;
	if (!check_lfsr_pattern(1438200953ul)) return;
	if (!check_lfsr_pattern(3413783263ul)) return;
	if (!check_lfsr_pattern(1900517911ul)) return;
	if (!check_lfsr_pattern(1227909400ul)) return;
	if (!check_lfsr_pattern(276562754ul)) return;
	if (!check_lfsr_pattern(146878114ul)) return;
	if (!check_lfsr_pattern(615545407ul)) return;
	if (!check_lfsr_pattern(110497896ul)) return;
	if (!check_lfsr_pattern(74539250ul)) return;
	if (!check_lfsr_pattern(4197336575ul)) return;
	if (!check_lfsr_pattern(2280382233ul)) return;
	if (!check_lfsr_pattern(542894183ul)) return;
	if (!check_lfsr_pattern(3978544245ul)) return;
	if (!check_lfsr_pattern(2315909796ul)) return;
	if (!check_lfsr_pattern(3736286001ul)) return;
	if (!check_lfsr_pattern(2876690683ul)) return;
	if (!check_lfsr_pattern(215559886ul)) return;
	if (!check_lfsr_pattern(539179291ul)) return;
	if (!check_lfsr_pattern(537678650ul)) return;
	if (!check_lfsr_pattern(4001405270ul)) return;
	if (!check_lfsr_pattern(2169216599ul)) return;
	if (!check_lfsr_pattern(4036891097ul)) return;
	if (!check_lfsr_pattern(1535452389ul)) return;
	if (!check_lfsr_pattern(2959727213ul)) return;
	if (!check_lfsr_pattern(4219363395ul)) return;
	if (!check_lfsr_pattern(1036929753ul)) return;
	if (!check_lfsr_pattern(2125248865ul)) return;
	if (!check_lfsr_pattern(3177905864ul)) return;
	if (!check_lfsr_pattern(2399307098ul)) return;
	if (!check_lfsr_pattern(3847634607ul)) return;
	if (!check_lfsr_pattern(27467969ul)) return;
	if (!check_lfsr_pattern(520563506ul)) return;
	if (!check_lfsr_pattern(381313790ul)) return;
	if (!check_lfsr_pattern(4174769276ul)) return;
	if (!check_lfsr_pattern(3932189449ul)) return;
	if (!check_lfsr_pattern(4079717394ul)) return;
	if (!check_lfsr_pattern(868357076ul)) return;
	if (!check_lfsr_pattern(2474062993ul)) return;
	if (!check_lfsr_pattern(1502682190ul)) return;
	if (!check_lfsr_pattern(2471230478ul)) return;
	if (!check_lfsr_pattern(85016565ul)) return;
	if (!check_lfsr_pattern(1427530695ul)) return;
	if (!check_lfsr_pattern(1100533073ul)) return;
	if (!check_fixed_pattern(0x55555555)) return;
	if (!check_fixed_pattern(0x33333333)) return;
	if (!check_fixed_pattern(0x0F0F0F0F)) return;
	if (!check_fixed_pattern(0x00FF00FF)) return;
	if (!check_fixed_pattern(0x0000FFFF)) return;
	if (!check_fixed_pattern(0xAAAAAAAA)) return;
	if (!check_fixed_pattern(0xCCCCCCCC)) return;
	if (!check_fixed_pattern(0xF0F0F0F0)) return;
	if (!check_fixed_pattern(0xFF00FF00)) return;
	if (!check_fixed_pattern(0xFFFF0000)) return;
	if (!check_fixed_pattern(0xFFFFFFFF)) return;
	if (!check_fixed_pattern(0x00000000)) return;
	Serial.printf(" test ran for %.2f seconds\n", (float)msec / 1000.0f);
	Serial.println("All memory tests passed :-)");
	memory_ok = true;
}

bool fail_message(volatile uint32_t *location, uint32_t actual, uint32_t expected)
{
	Serial.printf(" Error at %08X, read %08X but expected %08X\n",
		(uint32_t)location, actual, expected);
	return false;
}

// fill the entire RAM with a fixed pattern, then check it
bool check_fixed_pattern(uint32_t pattern)
{
	volatile uint32_t *p;
	Serial.printf("testing with fixed pattern %08X\n", pattern);
	for (p = memory_begin; p < memory_end; p++) {
		*p = pattern;
	}
	arm_dcache_flush_delete((void *)memory_begin,
		(uint32_t)memory_end - (uint32_t)memory_begin);
	for (p = memory_begin; p < memory_end; p++) {
		uint32_t actual = *p;
		if (actual != pattern) return fail_message(p, actual, pattern);
	}
	return true;
}

// fill the entire RAM with a pseudo-random sequence, then check it
bool check_lfsr_pattern(uint32_t seed)
{
	volatile uint32_t *p;
	uint32_t reg;

	Serial.printf("testing with pseudo-random sequence, seed=%u\n", seed);
	reg = seed;
	for (p = memory_begin; p < memory_end; p++) {
		*p = reg;
		for (int i=0; i < 3; i++) {
			if (reg & 1) {
				reg >>= 1;
				reg ^= 0x7A5BC2E3;
			} else {
				reg >>= 1;
			}
		}
	}
	arm_dcache_flush_delete((void *)memory_begin,
		(uint32_t)memory_end - (uint32_t)memory_begin);
	reg = seed;
	for (p = memory_begin; p < memory_end; p++) {
		uint32_t actual = *p;
		if (actual != reg) return fail_message(p, actual, reg);
		//Serial.printf(" reg=%08X\n", reg);
		for (int i=0; i < 3; i++) {
			if (reg & 1) {
				reg >>= 1;
				reg ^= 0x7A5BC2E3;
			} else {
				reg >>= 1;
			}
		}
	}
	return true;
}

void loop()
{
	digitalWrite(13, HIGH);
	delay(100);
	if (!memory_ok) digitalWrite(13, LOW); // rapid blink if any test fails
	delay(100);
}
