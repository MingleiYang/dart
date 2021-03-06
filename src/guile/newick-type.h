#ifndef NEWICK_TYPE_INCLUDED
#define NEWICK_TYPE_INCLUDED

#include "util/guile-defs.h"
#include "tree/phylogeny.h"

// guile functions
void init_newick_type (void);
SCM make_newick_smob (const PHYLIP_tree& tree);

// guile smobs
// cast method
PHYLIP_tree* newick_cast_from_scm (SCM tree_smob);

#endif /* NEWICK_TYPE_INCLUDED */
