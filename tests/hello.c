
#include <stdio.h>

#include "rio.h"

int main() {
	puts(rio(greet));
	puts(rio_lang(greet, en));
	puts(rio_lang(greet, jp));
}
