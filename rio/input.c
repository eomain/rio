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
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "rio/lang.h"
#include "libcx/vec.h"

enum type {
	ID,
	STRING,
	LBRACE,
	RBRACE,
	BSLASH,
	EQUAL,
	NEWLINE
};

struct token {
	enum type type;
	const char *value;
};

static inline void token_push(vec v, enum type type, const char *value)
{
	move(v, ((struct token) {
		.type = type,
		.value = value
	}));
}

struct lex {
	const char *input;
};

static void lex_init(struct lex *lex, const char *input)
{
	lex->input = input;
}

static char lex_read(struct lex *lex)
{
	return *lex->input;
}

static char lex_next(struct lex *lex)
{
	return *lex->input++;
}

static bool lex_has_next(struct lex *lex)
{
	return (*lex->input) != '\0';
}

static const char *get_string(const char *src, size_t n)
{
	void *p = malloc(n + 1);
	if (!p)
		abort();
	memset(p, '\0', n + 1);
	memcpy(p, src, n);
	return p;
}

static vec scan(const char *input)
{
	vec v = vec();
	struct lex lex;
	lex_init(&lex, input);
	
	char c;
	const char *p;
	while (lex_has_next(&lex)) {
		p = lex.input;
		c = lex_next(&lex);
		if (c == '\n') {
			token_push(v, NEWLINE, NULL);
			continue;
		}
		if (isspace(c))
			continue;
		if (c == '#') {
			while (lex_has_next(&lex)) {
				if (lex_next(&lex) == '\n')
					break;
			}
			continue;
		}
		if (c == '=') {
			token_push(v, EQUAL, NULL);
			continue;
		}
		if (c == '[') {
			token_push(v, LBRACE, NULL);
			continue;
		}
		if (c == ']') {
			token_push(v, RBRACE, NULL);
			continue;
		}
		if (c == '\\') {
			token_push(v, BSLASH, NULL);
			continue;
		}
		if (c == '\"') {
			size_t n = 1;
			while (lex_has_next(&lex)) {
				if (lex_next(&lex) == '\"') {
					token_push(v, STRING, get_string(p, n + 1));
					break;
				}
				n++;
			}
			continue;
		}
		if (isalpha(c)) {
			size_t n = 1;
			do {
				c = lex_read(&lex);
				if (!(c == '_' || isalnum(c))) {
					token_push(v, ID, get_string(p, n));
					break;
				}
				lex_next(&lex);
				n++;
			} while (lex_has_next(&lex));
			continue;
		}
		
		destroy(v);
		return NULL;
	}
	
	return v;
}

struct parser {
	size_t index;
	size_t max;
	vec tokens;
};

static void parser_init(struct parser *p, vec tokens)
{
	p->index = 0;
	p->max = len(tokens);
	p->tokens = tokens;
}

static struct token *parser_read(struct parser *p)
{
	return ptr(get(p->tokens, p->index));
}

static struct token *parser_next(struct parser *p)
{
	return ptr(get(p->tokens, p->index++));
}

static bool parser_has_next(struct parser *p)
{
	return p->index < p->max;
}

static bool parser_end(struct parser *p)
{
	return p->index == p->max;
}

static inline struct token *parser_assert(struct parser *p, enum type type)
{
	struct token *t;
	if (!parser_has_next(p))
		return NULL;
	t = parser_next(p);
	if (t->type != type)
		return NULL;
	return t;
}

static inline struct token *parser_assert_read(struct parser *p, enum type type)
{
	struct token *t;
	if (!parser_has_next(p))
		return NULL;
	t = parser_read(p);
	if (t->type != type)
		return NULL;
	parser_next(p);
	return t;
}

static inline bool parser_assert_type(struct parser *p, enum type type)
{
	struct token *t;
	if (!parser_has_next(p))
		return false;
	t = parser_read(p);
	if (t->type != type)
		return false;
	return true;
}

static inline int parse_nl(struct parser *p)
{
	if (!parser_end(p)) {
		if (!parser_assert(p, NEWLINE))
			return -1;
	}
	return 0;
}

static int parse_pair(struct parser *p, struct entry *e)
{
	struct token *t;
	const char *lang, *value;
	
	if (!(t = parser_assert(p, ID)))
		return -1;
	lang = t->value;
	if (!parser_assert(p, EQUAL))
		return -1;
	if (!(t = parser_assert(p, STRING)))
		return -1;
	value = t->value;
	if (parse_nl(p) != 0)
		return -1;
		
	entry_add(e, pair_new(lang, value));
		
	return 0;
}

static int parse_entry(struct parser *p, struct lang *l)
{
	struct token *t;
	
	if (!parser_assert(p, LBRACE))
		return -1;
	if (!(t = parser_assert(p, ID)))
		return -1;
	if (!parser_assert(p, RBRACE))
		return -1;
	if (parse_nl(p) != 0)
		return -1;
	
	struct entry e = entry_new(t->value);
	
	while (parser_has_next(p) && !parser_assert_type(p, LBRACE))
		parse_pair(p, &e);
	
	lang_add(l, e);
	
	return 0;
}

static int parse(struct lang *l, vec tokens)
{
	struct parser parser;
	parser_init(&parser, tokens);
	
	while (parser_has_next(&parser)) {
		parser_assert_read(&parser, NEWLINE);
		if (parse_entry(&parser, l))
			return -1;
	}
	
	return 0;
}

int input(const char *name, struct lang *l)
{
	FILE *f = fopen(name, "r");
	if (!f)
		return -1;
		
	char const *input;
	if (fseek(f, 0, SEEK_END))
		return -1;
	size_t len = ftell(f);
	if (fseek(f, 0, SEEK_SET))
		return -1;
	input = malloc((sizeof *input) * (len + 1));
	if (!input)
		return -1;
	memset((void *) input, '\0', len + 1);
	fread((void *) input, sizeof *input, len,  f);
	
	vec tokens = scan(input);
	return parse(l, tokens);
}
