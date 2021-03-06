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

#include <ctype.h>
#include <stdio.h>

#include "rio/rio.h"
#include "rio/lang.h"
#include "rio/output.h"
#include "libcx/vec.h"

static void premable(FILE *f)
{
	fprintf(f, "/* THIS FILE WAS AUTO GENERATED BY RIO. PLEASE DO NOT MODIFY */\n\n");
}

static void guard_start(FILE *f, const char *name)
{
	fprintf(f, "#ifndef __RIO__AUTOGEN__%s__H\n", name);
	fprintf(f, "#define __RIO__AUTOGEN__%s__H\n", name);
}

static void guard_end(FILE *f)
{
	fprintf(f, "#endif");
}

static void define(FILE *f, const char *id, const char *value, const char *lang)
{
	fprintf(f, "#define __RIO__%s__%s__ %s\n", id, lang, value);
}

static void define_default(FILE *f, const char *lang)
{
	fprintf(f, "#define rio(id) rio_lang(id, %s)\n", lang);
}

static void define_default_lang(FILE *f, const char *lang)
{
	fprintf(f, "#define __RIO__DEFAULT__LANG__ %s\n", lang);
}

static void define_lang(FILE *f)
{
	fprintf(f, "#define rio_lang(id, lang) __RIO__ ## id ## __ ## lang ## __\n");
}

static void body(FILE *f, const char *lg, struct lang *l)
{
	fprintf(f, "\n");
	define_default_lang(f, lg);
	define_default(f, lg);
	define_lang(f);
	
	const char *id, *lang, *value;
	for (size_t i = 0; i < len(l->entries); i++) {
		struct entry *e = ptr(get(l->entries, i));
		id = e->id;
		for (size_t i = 0; i < len(e->pairs); i++) {
			struct pair *p = ptr(get(e->pairs, i));
			lang = p->lang;
			value = p->value;
			define(f, id, value, lang);
		}
	}
	
	fprintf(f, "\n");
}

int output(const char *name, const char *lg, struct lang *l)
{
	FILE *f = fopen(name, "w");
	if (!f)
		return -1;
	
	premable(f);
	guard_start(f, "");
	body(f, lg, l);
	guard_end(f);
	
	fclose(f);
	return 0;
}
