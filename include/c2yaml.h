#ifndef c2yaml_h
#define c2yaml_h

#include <yaml.h>

typedef int yaml_id_t;

yaml_id_t i2yaml(yaml_document_t *doc,int );
yaml_id_t f2yaml(yaml_document_t *doc,float );
yaml_id_t d2yaml(yaml_document_t *doc,double );
yaml_id_t s2yaml(yaml_document_t *doc,char *);
yaml_id_t i_array2yaml(yaml_document_t *doc, const int *arr, int n);
yaml_id_t d_array2yaml(yaml_document_t *doc, const double *arr, int n);

#endif
