// More functions and changes will be made in the future

#include <stdio.h>
#include <stdlib.h>

#define EPSILON 10e-9

void mem_err(void *ptr);
void value_err(void *ptr);

typedef struct {
	size_t rows, cols;
	double **data;
} Matrix;

typedef struct {
	size_t len;
	double *data;
} Vector;

void define_matrix(Matrix* matrix, double entries[], size_t len);
void define_vector(Vector* vector, double entries[], size_t len);
void free_matrix(Matrix* matrix);
void free_vector(Vector* vector);
void print_matrix(Matrix* matrix, int precision);
void swap_rows(Matrix* matrix, int a, int b);
void swap_cols(Matrix* matrix, int a, int b);

Matrix* create_matrix(size_t rows, size_t cols);
Matrix* null_matrix(size_t rows, size_t cols);
Matrix* id_matrix(size_t n);
Matrix* vector_to_matrix(Vector* vector);
Matrix* copy_matrix(Matrix* matrix);
Matrix* symmetric_matrix(Matrix* matrix);
Matrix* transpose_matrix(Matrix* matrix);
Matrix* inverse_matrix(Matrix* matrix);
Matrix* add_matrix(Matrix* a, Matrix* b);
Matrix* sub_matrix(Matrix* a, Matrix* b);
Matrix* mult_matrix(Matrix* a, Matrix* b);

Vector* create_vector(size_t len);
Vector* null_vector(size_t len);
Vector* matrix_to_vector(Matrix* matrix);
Vector* matrix_vector_mul(Matrix* matrix, Vector* vector);

int is_square_matrix(Matrix* matrix);
int determinant_matrix(Matrix* matrix);
int is_null_matrix(Matrix* matrix, double epsilon);
int is_id_matrix(Matrix* matrix, double epsilon);
int has_null_row(Matrix* matrix, double epsilon);
int has_null_col(Matrix* matrix, double epsilon);
int equal_matrix(Matrix* a, Matrix* b, double epsilon);

double abs_d(double number);

void mem_err(void *ptr) {
	if (ptr == NULL) {
		printf("Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}
}

void value_err(void *ptr) {
	if (ptr == NULL) {
		printf("Value error\n");
		exit(EXIT_FAILURE);
	}
}

void define_matrix(Matrix* matrix, double entries[], size_t len) {
	if (len != matrix->rows * matrix->cols) value_err(NULL);
	for (int i = 0; i < matrix->rows; i++)
		for (int j = 0; j < matrix->cols; j++)
			matrix->data[i][j] = entries[i*matrix->cols + j];
}

void define_vector(Vector* vector, double entries[], size_t len) {
	if (vector->len != len) value_err(NULL);
	for (int i = 0; i < vector->len; i++) vector->data[i] = entries[i];
}

void free_matrix(Matrix* matrix) {
	for (int i = 0; i < matrix->rows; i++) free(matrix->data[i]);
	free(matrix->data);
	free(matrix);
}

void free_vector(Vector* vector) {
	free(vector->data);
	free(vector);
}

void print_matrix(Matrix* matrix, int precision) {
	for (int i = 0; i < matrix->rows; i++) {
		printf("( ");
		for (int j = 0; j < matrix->cols; j++) {
			printf("%.*lf ", precision, matrix->data[i][j]);
		}
		printf(")\n");
	}
}

void swap_rows(Matrix* matrix, int a, int b) {
	if (a < 0 || b < 0 || a >= matrix->rows || b >= matrix->rows) value_err(NULL);
	for (int j = 0; j < matrix->cols; j++) {
		double temp = matrix->data[a][j];
		matrix->data[a][j] = matrix->data[b][j];
		matrix->data[b][j] = temp;
	}
}
void swap_cols(Matrix* matrix, int a, int b) {
	if (a < 0 || b < 0 || a >= matrix->cols || b >= matrix->cols) value_err(NULL);
	for (int i = 0; i < matrix->rows; i++) { 
		double temp = matrix->data[i][a];
		matrix->data[i][a] = matrix->data[i][b];
		matrix->data[i][b] = temp;
	} 
}

Matrix* create_matrix(size_t rows, size_t cols) { 
	if (rows < 1 || cols < 1) return NULL;

	Matrix* matrix = (Matrix*)malloc(sizeof(Matrix));
	mem_err(matrix);

	matrix->rows = rows;
	matrix->cols = cols;

	matrix->data = (double**)malloc(rows * sizeof(double*));
	mem_err(matrix->data);

	for (int i = 0; i < rows; i++) {
		matrix->data[i] = (double*)malloc(cols * sizeof(double));
		mem_err(matrix->data[i]);
	}

	return matrix;
}

Matrix* null_matrix(size_t rows, size_t cols) { 
	Matrix* matrix = create_matrix(rows, cols);
	double entries[rows*cols];
	for (int i = 0; i < rows*cols; i++) entries[i] = 0;
	define_matrix(matrix, entries, rows*cols);
	return matrix;
}

Matrix* id_matrix(size_t n) {
	Matrix* id = create_matrix(n,n);
	for (int i = 0; i < n; i++) id->data[i][i] = 1.;
	return id;
}

Matrix* vector_to_matrix(Vector* vector) {
	Matrix* vector_mat = create_matrix(vector->len,1);
	define_matrix(vector_mat, vector->data, vector->len);
	return vector_mat;
}

Matrix* copy_matrix(Matrix* matrix) {
	Matrix* copy = create_matrix(matrix->rows, matrix->cols);
	for (int i = 0; i < matrix->rows; i++)
		for (int j = 0; j < matrix->cols; j++)
			copy->data[i][j] = matrix->data[i][j];
	return copy;
}

Matrix* symmetric_matrix(Matrix* matrix) {
	Matrix* symetric = create_matrix(matrix->rows, matrix->cols);
	for (int i = 0; i < symetric->rows; i++)
		for (int j = 0; j < symetric->cols; j++)
			symetric->data[i][j] = -matrix->data[i][j];
	return symetric;
}

Matrix* transpose_matrix(Matrix* matrix) {
	Matrix* transpose = create_matrix(matrix->cols, matrix->rows);
	for (int i = 0; i < matrix->rows; i++)
		for (int j = 0; j < matrix->cols; j++)
			transpose->data[j][i] = matrix->data[i][j];
	return transpose;
}

Matrix* inverse_matrix(Matrix* matrix) {
	if (!is_square_matrix(matrix)) return NULL;
	if (!determinant_matrix(matrix)) return NULL;
	if (is_id_matrix(matrix, EPSILON)) return id_matrix(matrix->rows);

	Matrix* copy = copy_matrix(matrix);
	Matrix* id = id_matrix(matrix->rows);

	for (int j = 0; j < copy->cols; j++) {		
		if (is_id_matrix(copy, EPSILON)) break;

		double a_jj = copy->data[j][j];

		if (!a_jj) {
			int row = j;
			for (int i = j; i < copy->rows; i++)
				if (!(copy->data[row][j]) || (!copy->data[i][j] && abs_d(copy->data[i][j] - 1) < abs_d(copy->data[row][j] - 1))) row = i;
			a_jj = copy->data[row][j];
			swap_rows(copy, row, j);
		}

		if (a_jj != 1) for (int k = j; k < copy->cols; k++) copy->data[j][k] /= a_jj;	
		
		for (int i = 0; i < copy->rows; i++)
			for (int k = j; k < copy->cols; k++)
				if (k != i)
					copy->data[i][k] -= copy->data[j][k] * copy->data[i][j]; 
	}
	return id;
}

Matrix* add_matrix(Matrix* a, Matrix* b) {
	if (!(a->rows == b->rows && a->cols == b->cols)) return NULL;
	Matrix* c = create_matrix(a->rows,a->cols);
	for (int i = 0; i < c->rows; i++)
		for (int j = 0; j < c->cols; j++)
			c->data[i][j] = a->data[i][j] + b->data[i][j];
	return c;
}

Matrix* sub_matrix(Matrix* a, Matrix* b) { return add_matrix(a, symmetric_matrix(b)); }

Matrix* mult_matrix(Matrix* a, Matrix* b) {
	if (a->cols - b->rows) return NULL;
	if (is_null_matrix(a, EPSILON) || is_null_matrix(b, EPSILON)) return null_matrix(a->rows,b->cols);
	if (is_id_matrix(a, EPSILON)) return copy_matrix(b);
	if (is_id_matrix(b, EPSILON)) return copy_matrix(a);
	Matrix* c = create_matrix(a->rows,b->cols);
	for (int i = 0; i < c->rows; i++) {
		for (int j = 0; j < c->cols; j++) {
			c->data[i][j] = 0;
			for (int k = 0; j < a->cols; j++)
				c->data[i][j] += a->data[i][k] * b->data[k][j];
		} 
	}
	return c;
}

Vector* create_vector(size_t len) {
	if (!len) value_err(NULL);
	Vector* vector = (Vector*)malloc(sizeof(Vector));
	mem_err(vector);

	vector->len = len;
	vector->data = (double*)malloc(vector->len * sizeof(double));
	mem_err(vector->data);

	return vector;
}

Vector* null_vector(size_t len) {
	Vector* vector = create_vector(len);
	for (int i = 0; i < vector->len; i++) vector->data[i] = 0;
	return vector;
}

Vector* matrix_to_vector(Matrix* matrix) {
    if (matrix->cols != 1) value_err(NULL);
    Vector* vector = create_vector(matrix->rows);
    for (int i = 0; i < matrix->rows; i++) vector->data[i] = matrix->data[i][0];
    return vector;
}

Vector* matrix_vector_mul(Matrix* matrix, Vector* vector) {
    if (matrix->cols != vector->len) value_err(NULL);
    Matrix* vec_mat = vector_to_matrix(vector);
    Matrix* result = mult_matrix(matrix, vec_mat);
    Vector* result_vector = matrix_to_vector(result);
    free_matrix(vec_mat);
    free_matrix(result);
    return result_vector;
}

int is_square_matrix(Matrix* matrix) { return matrix->rows == matrix->cols; }

int determinant_matrix(Matrix* matrix) {
	if (!is_square_matrix(matrix)) value_err(NULL);
	if (matrix->rows == 1) return matrix->data[0][0];
	if (has_null_row(matrix, EPSILON) || has_null_col(matrix, EPSILON)) return 0;
	if (is_null_matrix(matrix, EPSILON)) return 0;
	if (is_id_matrix(matrix, EPSILON)) return 1;
	if (is_id_matrix(symmetric_matrix(matrix), EPSILON)) return -1;

	Matrix* determinant = copy_matrix(matrix);
	
	int apply_la_place(Matrix* mat) {
		if (mat->rows == 2) return mat->data[0][0] * mat->data[1][1] - mat->data[1][0] * mat->data[0][1];
		
		int det = 0;
		for (int j = 0; j < mat->cols; j++) {
			Matrix* sub_mat = create_matrix(mat->rows - 1, mat->rows - 1);
			int offset = 0;
			for (int q = 0; q < mat->cols; q++)
				for (int p = 0; p < sub_mat->rows; p++)
					if (q != j) sub_mat->data[p][q-offset] = mat->data[p+1][q];
					else offset = 1;
			int sign = j%2 == 0 ? 1 : -1;
			det += sign * mat->data[0][j] * apply_la_place(sub_mat);
			free_matrix(sub_mat);
		}
		return det;
	}

	return apply_la_place(determinant);
}	

int is_null_matrix(Matrix* matrix, double epsilon) {
	Matrix* null_mat = null_matrix(matrix->rows,matrix->cols);
	int test = equal_matrix(matrix, null_mat, epsilon);
	free(null_mat);
	return test;
}

int is_id_matrix(Matrix* matrix, double epsilon) { 
	if (matrix->rows != matrix->cols) return 0;
	Matrix* id = id_matrix(matrix->rows);
	int test = equal_matrix(matrix, id, epsilon);
	free(id);
	return test;
}

int has_null_row(Matrix* matrix, double epsilon) {
	for (int i = 0; i < matrix->rows; i++) {
		unsigned count = 0;
		for (int j = 0; j < matrix->cols; j++)
			if (abs_d(matrix->data[i][j]) < epsilon) count++;
		if (count == matrix->cols) return 1;
	}
	return 0;
}

int has_null_col(Matrix* matrix, double epsilon) {
	for (int j = 0; j < matrix->cols; j++) {
		unsigned count = 0;
		for (int i = 0; i < matrix->rows; i++)
			if (abs_d(matrix->data[i][j]) < epsilon) count++;
		if (count == matrix->rows) return 1;
	}
	return 0;
}

int equal_matrix(Matrix* a, Matrix* b, double epsilon) {
	if (a->rows != b->rows || a->cols != b->cols) return 0;
	for (int i = 0; i < a->rows; i++)
		for (int j = 0; j < a->cols; j++)
			if (abs_d(a->data[i][j] - b->data[i][j]) > epsilon) return 0;
	return 1;
}

double abs_d(double number) { return number < 0 ? -number : number; } 

