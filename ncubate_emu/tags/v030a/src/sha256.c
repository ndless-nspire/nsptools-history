#include <stdlib.h>
#include <string.h>
#include "emu.h"

static u32 hash_state[8];
static u32 hash_block[16];

#define ROR(x, y) ((x) >> (y) | (x) << (32 - (y)))

static inline void initialize() {
	static const u32 initial_state[8] = {
		0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
		0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
	};
	memcpy(hash_state, initial_state, 32);
}

static void process_block() {
	static const u32 k[64] = {
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
	};

	u32 a, b, c, d, e, f, g, h;
	u32 w[64];
	int i;

	memcpy(w, hash_block, 64);
	for (i = 16; i < 64; i++) {
		u32 s0 = ROR(w[i-15], 7) ^ ROR(w[i-15], 18) ^ (w[i-15] >> 3);
		u32 s1 = ROR(w[i-2], 17) ^ ROR(w[i-2], 19) ^ (w[i-2] >> 10);
		w[i] = w[i-16] + s0 + w[i-7] + s1;
	}

	a = hash_state[0];
	b = hash_state[1];
	c = hash_state[2];
	d = hash_state[3];
	e = hash_state[4];
	f = hash_state[5];
	g = hash_state[6];
	h = hash_state[7];

	for (i = 0; i < 64; i++) {
		u32 s0 = ROR(a, 2) ^ ROR(a, 13) ^ ROR(a, 22);
		u32 maj = (a & b) ^ (a & c) ^ (b & c);
		u32 t2 = s0 + maj;
		u32 s1 = ROR(e, 6) ^ ROR(e, 11) ^ ROR(e, 25);
		u32 ch = (e & f) ^ (~e & g);
		u32 t1 = h + s1 + ch + k[i] + w[i];

		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

	hash_state[0] += a;
	hash_state[1] += b;
	hash_state[2] += c;
	hash_state[3] += d;
	hash_state[4] += e;
	hash_state[5] += f;
	hash_state[6] += g;
	hash_state[7] += h;
}

u32 sha256_read_word(u32 addr) {
	logprintf(LOG_HASH, "read %08x\n", addr);
	switch (addr & 0x3FFFFFF) {
		case 0x000: return 0; // bit 0 = busy
		case 0x008: return 0x100; //?
		case 0x060: case 0x064: case 0x068: case 0x06C:
		case 0x070: case 0x074: case 0x078: case 0x07C:
			return hash_state[addr >> 2 & 7];
	}
	return bad_read_word(addr);
}

void sha256_write_word(u32 addr, u32 value) {
	logprintf(LOG_HASH, "write %08x %08x\n", addr, value);
	switch (addr & 0x3FFFFFF) {
		case 0x000:
			if (value == 0x10 || value == 0x00) {
				initialize();
			} else if (value == 0x0E || value == 0x0A) {
				// 0E = first, 0A = not first.
				process_block();
			} else {
				break;
			}
			return;
		case 0x008: return;
		case 0x010: case 0x014: case 0x018: case 0x01C:
		case 0x020: case 0x024: case 0x028: case 0x02C:
		case 0x030: case 0x034: case 0x038: case 0x03C:
		case 0x040: case 0x044: case 0x048: case 0x04C:
			hash_block[((addr & 0x3FFFFFF) - 0x10) >> 2] = value;
			return;
	}
	bad_write_word(addr, value);
}

struct sha256_saved_state {
	u32 hash_state[8];
	u32 hash_block[16];
};

void *sha256_save_state(size_t *size) {
	*size = sizeof(struct sha256_saved_state);
	struct sha256_saved_state *state = malloc(*size);
	memcpy(&state->hash_state, hash_state, sizeof(hash_state));
	memcpy(&state->hash_block, hash_block, sizeof(hash_block));
	return state;
}

void sha256_reload_state(void *state) {
	struct sha256_saved_state *_state = (struct sha256_saved_state *)state;
	memcpy(hash_state, &_state->hash_state, sizeof(hash_state));
	memcpy(hash_block, &_state->hash_block, sizeof(hash_block));
}