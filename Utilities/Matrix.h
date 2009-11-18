
/* header file for matrix function and MATRIX structure */
// This should probably be turned into a DLL
#ifndef _MATRIX_H_
#define _MATRIX_H_

typedef struct
{
	 int element_size;
	 int rows;
	 int cols;
	 char **ptr;
} MATRIX;

	MATRIX *matrix_allocate(int, int, int);
	void matrix_free(MATRIX *);
	void matrix_print(MATRIX *);
	MATRIX *matrix_scale(MATRIX *,double);
	MATRIX *matrix_crop(MATRIX *,int,int,int,int);
	MATRIX *matrix_add(MATRIX *,MATRIX *);
	MATRIX *matrix_sub(MATRIX *,MATRIX *);
	MATRIX *matrix_mult_pwise(MATRIX *,MATRIX *);
	MATRIX *matrix_mult(MATRIX *,MATRIX *);
	MATRIX *matrix_transpose(MATRIX *);
	MATRIX *matrix_invert(MATRIX *);
	double matrix_det(MATRIX *);
	//MATRIX *matrix_read(char *);

#endif