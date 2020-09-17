/* Copyright 2020 eomain
   this program is licensed under the 2-clause BSD license
   see COPYING for the full license info

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "rio/rio.h"
#include "rio/input.h"
#include "rio/lang.h"
#include "rio/output.h"
#include "libcx/queue.h"

struct rio_option {
	const char *out;
	const char *path;
	const char *lang;
};

static void rio_option_default(struct rio_option *opt)
{
	opt->out = NULL;
	opt->path = NULL;
	opt->lang = "en";
}

static void rio_option_run(struct rio_option *opt)
{
	struct lang l = lang_new();
	input(opt->path, &l);
	if (output(opt->out, opt->lang, &l) != 0) {
		printf("%s: %s: %s\n", NAME, rio(error), strerror(errno));
		exit(EXIT_FAILURE);
	}
}

static inline const char *getarg_or(queue q, const char *msg)
{
	if (empty(q)) {
		printf("%s: %s: %s\n", NAME, rio(argument), msg);
		exit(EXIT_SUCCESS);
	}
	return ptr(dequeue(q));
}

int main(int argc, const char *argv[])
{
	queue args = queue();
	for (size_t i = 1; i < argc; i++)
		enqueue(args, argv[i]);
		
	if (empty(args)) {
		printf("%s: %s\n", NAME, rio(about));
		exit(EXIT_SUCCESS);
	}
	
	struct rio_option opt;
	rio_option_default(&opt);
	
	const char *q;
	while (!empty(args)) {
		q = ptr(dequeue(args));
		
		if (*q == '-') {
			if (!strcmp(q, "-v")) {
				printf("%s: %s\n", NAME, VERSION);
				exit(EXIT_SUCCESS);
			} else if (!strcmp(q, "-h")) {
				printf("%s: %s\n%s\n", NAME, VERSION, rio(help));
				exit(EXIT_SUCCESS);
			} else if (!strcmp(q, "-l")) {
				opt.lang = q;
			} else if (!strcmp(q, "-o")) {
				opt.out = getarg_or(args, rio(output));
			} else {
				printf("%s: %s: '%s'\n", NAME, rio(option), q);
				exit(EXIT_FAILURE);
			}
		} else {
			if (opt.path) {
				printf("%s: %s: %s: '%s' %s '%s'\n", NAME, rio(error), rio(multiple), opt.path, rio(and), q);
				exit(EXIT_FAILURE);
			}
			opt.path = q;
		}
	}
	
	if (!opt.out)
		opt.out = "rio.h";
	
	rio_option_run(&opt);
	destroy(args);
	return 0;
}
