 /*
    COPYRIGHT DISCLAIMER

    Vincent Le Guilloux, Peter Schmidtke and Pierre Tuffery, hereby
	claim all copyright interest in the program “fpocket” (which
	performs protein cavity detection) written by Vincent Le Guilloux and Peter
	Schmidtke.

    Vincent Le Guilloux  01 Decembre 2012
    Peter Schmidtke      01 Decembre 2012
    Pierre Tuffery       01 Decembre 2012

    GNU GPL

    This file is part of the fpocket package.

    fpocket is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    fpocket is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with fpocket.  If not, see <http://www.gnu.org/licenses/>.

**/


#ifndef DH_VORONOI
#define DH_VORONOI

// ---------------------------------INCLUDES----------------------------------*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include "rpdb.h"
#include "writepdb.h"
#include "calc.h"
#include "utils.h"


#include "../src/qhull/qvoronoi.h"
#include "../src/qhull/qconvex.h"

#include "memhandler.h"

/* ----------------------------------MACROS--------------------------------- */

#define M_VORONOI_SUCCESS 0 /**< alpha sphere type - hydrophobic alpha sphere */
#define M_APOLAR_AS 0 /**< alpha sphere type - hydrophilic alpha sphere */
#define M_POLAR_AS 1 
#define M_PREC_TOLERANCE 1e-3 /**< tolerance for coordinate imprecion during alpha sphere search*/

#define M_BUFSIZE 1e6   /**< buffer size*/
#define M_N_REPLICAS 1  /**< currently not used anymore : number of times we do the voronoi tesselation for translated coordinates to avoid precision problems with qhull */

/* --------------------------------STRUCTURES-------------------------------- */

/**
 Container of the Voronoi vertice
 */
typedef struct s_vvertice 
{
	int resid ; /**< residue ID*/
	int id, /**< vertice ID*/
            seen,       /**< Say if we have seen this vertice during a neighbor search */
            qhullId,    /**< ID of the vertice in qhull output*/
            type ;	/**< 0 if apolar contacts, 1 if polar */

	float ray ;	/**< Ray of voronoi vertice */
	float x,	/**< X coord */
              y,	/**< Y coord */
	      z ;	/**< Z coord */
	
	int sort_x;		/**< Index in the sorted tab by X coord */
	int apol_neighbours;	/**< number of neighbouring apolar alpha spheres */

	int vneigh[4] ;         /**< vertice neighbours (4 contact atoms)*/
 	s_atm *neigh[4] ;	/**< The theorical 4 contacted atoms */
	
	float bary[3] ;         /**< Barycenter of the pocket */
        float electrostatic_energy;     /**<estimate of electrostatic energy around the alpha sphere */

} s_vvertice ;

/**
 vertice list container
 */
typedef struct s_lst_vvertice
{
	s_vvertice *vertices ;      /**< List of voronoi vertices */
        s_vvertice **pvertices ;  /**< list of pointers to vertices*/

        /* Indexes of heavy atoms used as input for qhull in the s_pdb list of atoms */
	int *h_tr;

        /* Size of h_tr */
	int n_h_tr;

	int *tr,    /**< translation of voronoi vertice indices*/
            nvert,  /**< no of vertices*/
	    qhullSize ; /**< number of vertices in qhull*/

} s_lst_vvertice ;


/**
 * clusterlib vertice info container
 */

typedef struct s_clusterlib_vertices
{
        double ** pos;          /**< Positions (xyz) of Voronoi vertices*/
        int ** mask;            /**< Mask for position array*/
        double weight[3];       /**< column (coordinate weights for distance matrix calculation*/
        int transpose;          /**< cluster by rows (transpose=0) or column(transpose=1)*/
        char dist;              /**< char defining distance measure used*/
        char method;            /**< char defining method used (clustering)*/
        

        
} s_clusterlib_vertices ;

/* -----------------------------PROTOTYPES----------------------------------- */

s_lst_vvertice* load_vvertices_DEPRECATED(s_pdb *pdb, int min_apol_neigh, 
				float ashape_min_size, float ashape_max_size,
                                float xshift,float yshift, float zshift) ;
s_lst_vvertice* load_vvertices(s_pdb *pdb, int min_apol_neigh, float asph_min_size, float asph_max_size,float xshift,float yshift,float zshift);


void fill_vvertices(s_lst_vvertice *lvvert, const char fpath[], s_atm *atoms, int natoms,
                           int min_apol_neigh, float asph_min_size, float asph_max_size,
                           float xshift, float yshift, float zshift,float avg_bfactor,s_pdb *pdb) ;


void add_missing_vvertices(s_lst_vvertice *lvvert, const char fpath[], s_atm *atoms, int natoms,
                           int min_apol_neigh, float asph_min_size, float asph_max_size,
                           float xshift, float yshift,float zshift, float avg_bfactor,s_pdb *pdb);

unsigned short check_if_similar_vertice_in_list(s_vvertice *v, s_lst_vvertice *lvvert);
void add_vertice_to_vertice_list(s_lst_vvertice *lvvert,s_vvertice *v);
void copy_vertice(s_vvertice *v1,s_vvertice *v2, int n);

float testVvertice(float xyz[3], int curNbIdx[4], s_atm *atoms, 
				   float min_asph_size, float max_asph_size, 
				   s_lst_vvertice *lvvert,
                                   float xshift, float yshift, float zshift,float avg_bfactor,s_pdb *pdb);

s_lst_vvertice *compare_vvertice_shifted_lists(s_lst_vvertice *lvvert,
        s_lst_vvertice *list_shifted,float xshift,float yshift,float zshift);


double **get_3d_array_from_vvertice_list(s_lst_vvertice *lvvert);
s_clusterlib_vertices *prepare_vertices_for_cluster_lib(s_lst_vvertice *lvvert,char c_method,char d_method);
void set_barycenter(s_vvertice *v) ;
int is_in_lst_vert(s_vvertice **lst_vert, int nb_vert, int v_id) ;
int is_in_lst_vert_p(s_vvertice **lst_vert, int nb_vert, s_vvertice *vert);

void write_pqr_vert(FILE *f, s_vvertice *v) ;
void write_pdb_vert(FILE *f, s_vvertice *v) ;
float get_verts_volume_ptr(s_vvertice **verts, int nvert, int niter,float correct);
float get_convex_hull_volume(s_vvertice **verts, int nvert);

void print_vvertices(FILE *f, s_lst_vvertice *lvvert) ;
void free_vert_lst(s_lst_vvertice *lvvert) ;

void transferClustersToVertices(int **clusterIds,s_lst_vvertice *lvert);

double frand_a_b(double a, double b);
int free_cluster_lib_vertices(s_clusterlib_vertices *clusterlib_vertices, int nvert);

#endif
