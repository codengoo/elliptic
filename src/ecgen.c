#include <pari/pari.h>
#include "cm/cm.h"
#include "exhaustive/exhaustive.h"
#include "io/output.h"
#include "util/timeout.h"
#include "util/random.h"

bool init(void) {
	// init PARI, 1GB stack, 1M primes
	pari_init(cfg->memory, 1000000);

	// init PARI PRNG
	if (!random_init()) return false;

	// init the signal handlers, etc. for timeout handling
	if (!timeout_init()) return false;
	
	// Fix the mysterious isprime bug.
	isprime(stoi(1));

	// open outfile
	if (!output_init()) return false;

	return true;
}

int quit(int status) {
	pari_close();

	timeout_quit();

	output_quit();

	return status;
}

int main(int argc, char *argv[]) {
	memset(cfg, 0, sizeof(config_t));
	
	cfg->bits = 160;
	cfg->field |= FIELD_PRIME;
	cfg->method |= METHOD_ANOMALOUS;
	cfg->random = RANDOM_ALL;
	cfg->prime = true;
	cfg->unique = true;
	cfg->count = 1;
	cfg->memory = 1000000000;
	cfg->threads = 1;
	cfg->thread_memory = cfg->bits * 2000000;
	cfg->format = FORMAT_JSON;

	if (!init()) {
		return quit(EXIT_FAILURE);
	}

	int status =  cm_do();

	return quit(status);
}
