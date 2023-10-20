#include <pari/pari.h>
#include "cm/cm.h"
#include "exhaustive/exhaustive.h"
#include "io/input.h"
#include "io/output.h"
#include "util/timeout.h"

static struct argp argp = {cli_options, cli_parse, cli_args_doc,
                           cli_doc,     0,         cli_filter};

bool init(void) {
	// init PARI, 1GB stack, 1M primes
	pari_init(cfg->memory, 1000000);

	// init PARI PRNG
	if (!random_init()) return false;

	// init the signal handlers, etc. for timeout handling
	if (!timeout_init()) return false;

	// set datadir if specified
	if (cfg->datadir) {
		default0("datadir", cfg->datadir);
	}
	
	// Fix the mysterious isprime bug.
	isprime(stoi(1));

	// open outfile
	if (!output_init()) return false;

	// open infile
	if (!input_init()) return false;

	return true;
}

int quit(int status) {
	pari_close();

	timeout_quit();

	output_quit();
	input_quit();

	return status;
}

int main(int argc, char *argv[]) {
	memset(cfg, 0, sizeof(config_t));
	if (!cli_init()) {
		return quit(EXIT_FAILURE);
	}
	argp_parse(&argp, argc, argv, 0, 0, cfg);
	cli_quit();

	if (!init()) {
		return quit(EXIT_FAILURE);
	}

	int status;
	if (cfg->method == METHOD_CM || cfg->method == METHOD_ANOMALOUS ||
	    cfg->method == METHOD_SUPERSINGULAR) {
		status = cm_do();
	}

	return quit(status);
}
