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

#ifndef RIO_LANG_H
#define RIO_LANG_H

#include "libcx/vec.h"

struct pair {
	const char *lang;
	const char *value;
};

static inline struct pair pair_new(const char *lang, const char *value)
{
	return (struct pair) {
		.lang = lang,
		.value = value
	};
}

struct entry {
	const char *id;
	vec pairs;
};

static inline struct entry entry_new(const char *id)
{
	return (struct entry) {
		.id = id,
		.pairs = vec()
	};
}

static inline void entry_add(struct entry *e, struct pair p)
{
	move(e->pairs, p);
}

struct lang {
	vec entries;
};

static inline struct lang lang_new(void)
{
	return (struct lang) {
		.entries = vec()
	};
}

static void inline lang_add(struct lang *l, struct entry e)
{
	move(l->entries, e);
}

#endif
