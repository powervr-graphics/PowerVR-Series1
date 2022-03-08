/* dlshade.h */

/* 
   this is actually implemented in rnshade.c but I've put this proto in
   here so dl* files don't have to inlcude loads of rn*.h files
*/

int SmoothPlaneDataPrecalc (CONV_SHADING_STRUCT *pShading,
                            sgl_vector pt1, sgl_vector pt2, sgl_vector pt3,
                            sgl_vector n1, sgl_vector n2, sgl_vector n3);


