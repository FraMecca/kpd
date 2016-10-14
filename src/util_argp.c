#include "util_argp.h"

void funct ()
{
	return;
}

bool version ()
{
	fprintf (stdout, "kpd 0.0.1\nLicense GPLV3+ <http://gnu.org/licenses/gpl.html>.\nThis is free software, you are welcome to contribute to the development.\n");
	return true;
}

int
parse_opt_filters (int k, char *arg, struct argp_state *state)
{
	char **args = NULL;
	int pos, n = 0, i;
	char *longKey = NULL;

	/*
	 * state is a struct that cointains argc and argv
	 */
	/*
	 * now find the key inside the option array, then iterate until you find the option or short option in argv
	 */
	i = 0;
	while (k != 0 && options[i].key != 0 && options[i].key != k) {
		// needed to find the long_option related to key
		++i;
	}
	if (options[i].key != 0) {
		longKey = strdup (options[i].name);
		 /*iterate argv till you find arg*/
		 /*arg is the first argument*/
		 /*iterate till you find the next option and save the arguments meanwhile*/
		for (i = 0; i < state->argc; ++i) {
			if ((strlen (state->argv[i]) > 2 && strcmp (longKey, state->argv[i] + 2) == 0)
					|| (strlen (state->argv[i]) > 1 && state->argv[i][1] == k)) {

				break;
				// found!
			}
		}
		// now count the arguments so that you can allocate args
		pos = i + 1;
		for (i = pos; i < state->argc; ++i)  {
			if (state->argv[i][0] == '-') {
				// it maybe an option (starts with '-') so stop here
				break;
			}
		}
		n = i - pos;
		args = malloc (n * sizeof (char *));
		for (i = pos; i < state->argc; ++i) {
			// now allocate arguments into arg
			if (state->argv[i][0] == '-') {
				break;
			}
			args[i - pos] = strdup (state->argv[i]);
		}
	}
	if (longKey != NULL) {
		free (longKey);
	}
	// now you have args and n

	/* now iterate function table till you find the key and
	 * call the related function
	 */
	i = 0;
	while (filters[i].key != 0 && filters[i].key != k) {
		i++;
	}
	if (filters[i].key != 0){
		filters[i].functionPtr (args, n);
	}
	// else the key was not found

	if (args != NULL) {
		for (i = 0; i < n; ++i) {
			free (args[i]);
		}
		free (args);
	}
	return 0;
}


int
parse_opt (int k, char *arg, struct argp_state *state)
{
	char **args = NULL;
	int pos, n = 0, i;
	char *longKey = NULL;

	/*
	 * state is a struct that cointains argc and argv
	 */
	/*
	 * now find the key inside the option array, then iterate until you find the option or short option in argv
	 */
	i = 0;
	while (k != 0 && options[i].key != 0 && options[i].key != k) {
		// needed to find the long_option related to key
		++i;
	}
	if (options[i].key != 0) {
		longKey = strdup (options[i].name);
		 /*iterate argv till you find arg*/
		 /*arg is the first argument*/
		 /*iterate till you find the next option and save the arguments meanwhile*/
		for (i = 0; i < state->argc; ++i) {
			if ((strlen (state->argv[i]) > 2 && strcmp (longKey, state->argv[i] + 2) == 0)
					|| (strlen (state->argv[i]) > 1 && state->argv[i][1] == k)) {

				break;
				// found!
			}
		}
		// now count the arguments so that you can allocate args
		pos = i + 1;
		for (i = pos; i < state->argc; ++i)  {
			if (state->argv[i][0] == '-') {
				// it maybe an option (starts with '-') so stop here
				break;
			}
		}
		n = i - pos;
		args = malloc (n * sizeof (char *));
		for (i = pos; i < state->argc; ++i) {
			// now allocate arguments into arg
			if (state->argv[i][0] == '-') {
				break;
			}
			args[i - pos] = strdup (state->argv[i]);
		}
	}
	if (longKey != NULL) {
		free (longKey);
	}
	// now you have args and n

	/* now iterate function table till you find the key and
	 * call the related function
	 */
	i = 0;
	while (functions[i].key != 0 && functions[i].key != k) {
		i++;
	}
	if (functions[i].key != 0){
		functions[i].functionPtr (args, n);
	}
	// else the key was not found

	if (args != NULL) {
		for (i = 0; i < n; ++i) {
			free (args[i]);
		}
		free (args);
	}
	return 0;
}


