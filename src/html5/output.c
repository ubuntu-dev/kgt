/*
 * Copyright 2014-2019 Katherine Flavel
 *
 * See LICENCE for the full copyright terms.
 */

/*
 * Railroad Diagram HTML5 Output
 *
 * Output a HTML page of inlined SVG diagrams
 */

#define _BSD_SOURCE

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include "../txt.h"
#include "../ast.h"
#include "../xalloc.h"

#include "../rrd/rrd.h"
#include "../rrd/pretty.h"
#include "../rrd/node.h"
#include "../rrd/rrd.h"
#include "../rrd/list.h"
#include "../rrd/tnode.h"

#include "io.h"

/* XXX */
extern struct dim svg_dim;
void svg_render_rule(const struct tnode *node, const char *base);

static void
output(const struct ast_rule *grammar, int xml)
{
	const struct ast_rule *p;

	printf(" <head>\n");
	if (xml) {
		printf("  <meta charset='UTF-8'/>\n");
	}

	printf("  <style>\n");
	printf("      rect, line, path { stroke-width: 1.5px; stroke: black; fill: transparent; }\n");
	printf("      rect, line, path { stroke-linecap: square; stroke-linejoin: rounded; }\n");
	printf("      path { fill: transparent; }\n");
	printf("      text.literal { font-family: monospace; }\n");
	printf("      a { fill: blue; }\n");
	printf("      a:hover rect { fill: aliceblue; }\n");
	printf("      h2 { font-size: inherit; font-weight: inherit; }\n");
	printf("      line.ellipsis { stroke-dasharray: 1 3.5; }\n");
	printf("      tspan.hex { font-family: monospace; font-size: 90%%; }\n");
	printf("      path.arrow { fill: black; }\n");
	printf("      svg { margin-left: 30px; }\n");
	printf("  </style>\n");

	printf(" </head>\n");
	printf("\n");

	printf(" <body>\n");

	for (p = grammar; p; p = p->next) {
		struct tnode *tnode;
		struct node *rrd;
		unsigned h, w;

		if (!ast_to_rrd(p, &rrd)) {
			perror("ast_to_rrd");
			return;
		}

		if (prettify) {
			rrd_pretty(&rrd);
		}

		tnode = rrd_to_tnode(rrd, &svg_dim);

		node_free(rrd);

		printf(" <section>\n");
		printf("  <h2><a name='%s'>%s:</a></h2>\n",
			p->name, p->name);

		h = (tnode->a + tnode->d + 1) * 10 + 5;
		w = (tnode->w + 6) * 10;

		printf("  <svg");
		if (xml) {
			printf(" xmlns='http://www.w3.org/2000/svg'");
		}
		printf(" height='%u' width='%u'>\n", h, w);
		svg_render_rule(tnode, "");
		printf("  </svg>\n");

		printf(" </section>\n");
		printf("\n");

		tnode_free(tnode);
	}

	printf(" </body>\n");
}

void
html5_output(const struct ast_rule *grammar)
{
	printf("<!DOCTYPE html>\n");
	printf("<html>\n");
	printf("\n");

	output(grammar, 0);

	printf("</html>\n");
}

void
xhtml5_output(const struct ast_rule *grammar)
{
	printf("<?xml version='1.0' encoding='utf-8'?>\n");
	printf("<!DOCTYPE html>\n");
	printf("<html xml:lang='en' lang='en'\n");
	printf("  xmlns='http://www.w3.org/1999/xhtml'\n");
	printf("  xmlns:xlink='http://www.w3.org/1999/xlink'>\n");
	printf("\n");

	output(grammar, 1);

	printf("</html>\n");
}

