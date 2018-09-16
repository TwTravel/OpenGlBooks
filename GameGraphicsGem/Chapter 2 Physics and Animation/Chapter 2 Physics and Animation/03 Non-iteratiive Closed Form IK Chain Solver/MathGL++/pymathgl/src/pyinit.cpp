/***************************************************************************
 *   Copyright (C) 2005 by Jacques Gasselin de Richebourg                  *
 *   jacquesgasselin@hotmail.com                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
extern "C"
{
#include <Python.h>
}
#include <GL/gl.h>
#include <MathGL++/GLMatrix.h>

using namespace mathglpp;

typedef struct {
    PyObject_HEAD
    GLMatrix4d* matrix;
} PyGLMatrix;

typedef struct {
    PyObject_HEAD
    GLVector3d* vector;
} PyGLVector3;

static PyObject* new_matrix(PyObject* self);

static PyObject* PyGLMatrix_loadIdentity(PyObject* self)
{
    GLMatrix4d* matrix = ((PyGLMatrix*)self)->matrix;
    matrix->loadIdentity();
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* PyGLMatrix_glMultMatrix(PyObject* self)
{
    GLMatrix4d* matrix = ((PyGLMatrix*)self)->matrix;
    glMultMatrixd(*matrix);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* PyGLMatrix_glLoadMatrix(PyObject* self)
{
    GLMatrix4d* matrix = ((PyGLMatrix*)self)->matrix;
    glLoadMatrixd(*matrix);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* PyGLMatrix_loadRotate(PyObject* self, PyObject* args)
{
    GLMatrix4d* matrix = ((PyGLMatrix*)self)->matrix;
    GLdouble angle,x,y,z;

    PyArg_ParseTuple(args, "dddd", &angle,&x,&y,&z);

    matrix->loadRotate(angle,x,y,z);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* PyGLMatrix_applyRotate(PyObject* self, PyObject* args)
{
    GLMatrix4d* matrix = ((PyGLMatrix*)self)->matrix;
    GLdouble angle,x,y,z;

    PyArg_ParseTuple(args, "dddd", &angle,&x,&y,&z);

    matrix->applyRotate(angle,x,y,z);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* PyGLMatrix_loadTranslate(PyObject* self, PyObject* args)
{
    GLMatrix4d* matrix = ((PyGLMatrix*)self)->matrix;
    GLdouble x,y,z;

    PyArg_ParseTuple(args, "ddd", &x,&y,&z);

    matrix->loadTranslate(x,y,z);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* PyGLMatrix_applyTranslate(PyObject* self, PyObject* args)
{
    GLMatrix4d* matrix = ((PyGLMatrix*)self)->matrix;
    GLdouble x,y,z;

    PyArg_ParseTuple(args, "ddd", &x,&y,&z);

    matrix->applyTranslate(x,y,z);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* PyGLMatrix_loadScale(PyObject* self, PyObject* args)
{
    GLMatrix4d* matrix = ((PyGLMatrix*)self)->matrix;
    GLdouble x,y,z;

    PyArg_ParseTuple(args, "ddd", &x,&y,&z);

    matrix->loadScale(x,y,z);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* PyGLMatrix_applyScale(PyObject* self, PyObject* args)
{
    GLMatrix4d* matrix = ((PyGLMatrix*)self)->matrix;
    GLdouble x,y,z;

    PyArg_ParseTuple(args, "ddd", &x,&y,&z);

    matrix->applyScale(x,y,z);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* PyGLMatrix_loadFrustum(PyObject* self, PyObject* args)
{
    GLMatrix4d* matrix = ((PyGLMatrix*)self)->matrix;
    GLdouble left,right,bottom,top,zNear,zFar;
    PyArg_ParseTuple(args, "dddddd", &left,&right,&bottom,&top,&zNear,&zFar);

    matrix->loadFrustum(left,right,bottom,top,zNear,zFar);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* PyGLMatrix_loadOrtho(PyObject* self, PyObject* args)
{
    GLMatrix4d* matrix = ((PyGLMatrix*)self)->matrix;
    GLdouble left,right,bottom,top,zNear,zFar;
    PyArg_ParseTuple(args, "dddddd", &left,&right,&bottom,&top,&zNear,&zFar);

    matrix->loadOrtho(left,right,bottom,top,zNear,zFar);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* PyGLMatrix_loadView(PyObject* self, PyObject* args)
{
    GLMatrix4d* matrix = ((PyGLMatrix*)self)->matrix;
    GLdouble fx, fy, fz, ux, uy, uz, sx, sy, sz;
    PyArg_ParseTuple(args, "ddddddddd", &fx,&fy,&fz,&ux,&uy,&uz,&sx,&sy,&sz);

    matrix->loadView(fx,fy,fz,ux,uy,uz,sx,sy,sz);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* PyGLMatrix_det(PyObject* self)
{
    GLMatrix4d* matrix = ((PyGLMatrix*)self)->matrix;

    return PyFloat_FromDouble(matrix->det());
}


static int PyGLMatrix_tp_init(PyObject* self, PyObject* args, PyObject* kwds)
{
    return 0;
}

static void PyGLMatrix_tp_dealloc(PyObject* self)
{
    PyGLMatrix* mat = (PyGLMatrix*)self;
    if(mat && mat->matrix)
        delete mat->matrix;    
    
    if(self)
        self->ob_type->tp_free(self);
}

static PyMethodDef PyGLMatrix_tp_methods[] =
{
    {"applyRotate", (PyCFunction)PyGLMatrix_applyRotate, METH_VARARGS },
    {"loadRotate", (PyCFunction)PyGLMatrix_loadRotate, METH_VARARGS },
    {"applyTranslate", (PyCFunction)PyGLMatrix_applyTranslate, METH_VARARGS },
    {"loadTranslate", (PyCFunction)PyGLMatrix_loadTranslate, METH_VARARGS },
    {"applyScale", (PyCFunction)PyGLMatrix_applyScale, METH_VARARGS },
    {"loadScale", (PyCFunction)PyGLMatrix_loadScale, METH_VARARGS },
    {"loadFrustum", (PyCFunction)PyGLMatrix_loadFrustum, METH_VARARGS },
    {"loadOrtho", (PyCFunction)PyGLMatrix_loadOrtho, METH_VARARGS },
    {"loadIdentity", (PyCFunction)PyGLMatrix_loadIdentity, METH_NOARGS },
    {"glMultMatrix", (PyCFunction)PyGLMatrix_glMultMatrix, METH_NOARGS },
    {"glLoadMatrix", (PyCFunction)PyGLMatrix_glLoadMatrix, METH_NOARGS },
    {"det", (PyCFunction)PyGLMatrix_det, METH_NOARGS },
    { NULL, NULL, NULL }
};

static PyObject* PyGLMatrix_add(PyObject* lhs, PyObject* rhs)
{
    GLMatrix4d* a = ((PyGLMatrix*)lhs)->matrix;
    PyObject* ret = new_matrix(NULL);
    GLMatrix4d* retMatrix = ((PyGLMatrix*)ret)->matrix;

    GLMatrix4d* b = ((PyGLMatrix*)rhs)->matrix;
    *retMatrix = (*a) + (*b);

    return ret;
}

static PyObject* PyGLMatrix_iadd(PyObject* lhs, PyObject* rhs)
{
    GLMatrix4d* a = ((PyGLMatrix*)lhs)->matrix;
    GLMatrix4d* b = ((PyGLMatrix*)rhs)->matrix;
    (*a) += (*b);

    return lhs;
}

static PyObject* PyGLMatrix_sub(PyObject* lhs, PyObject* rhs)
{
    GLMatrix4d* a = ((PyGLMatrix*)lhs)->matrix;
    PyObject* ret = new_matrix(NULL);
    GLMatrix4d* retMatrix = ((PyGLMatrix*)ret)->matrix;

    GLMatrix4d* b = ((PyGLMatrix*)rhs)->matrix;
    *retMatrix = (*a) - (*b);

    return ret;
}

static PyObject* PyGLMatrix_isub(PyObject* lhs, PyObject* rhs)
{
    GLMatrix4d* a = ((PyGLMatrix*)lhs)->matrix;
    GLMatrix4d* b = ((PyGLMatrix*)rhs)->matrix;
    (*a) -= (*b);

    return lhs;
}

static PyObject* PyGLMatrix_mul(PyObject* lhs, PyObject* rhs)
{
    GLMatrix4d* a = ((PyGLMatrix*)lhs)->matrix;
    PyObject* ret = new_matrix(NULL);
    GLMatrix4d* retMatrix = ((PyGLMatrix*)ret)->matrix;

    GLMatrix4d* b = ((PyGLMatrix*)rhs)->matrix;
    *retMatrix = (*a) * (*b);

    return ret;
}

static PyObject* PyGLMatrix_imul(PyObject* lhs, PyObject* rhs)
{
    GLMatrix4d* a = ((PyGLMatrix*)lhs)->matrix;
    GLMatrix4d* b = ((PyGLMatrix*)rhs)->matrix;
    (*a) *= (*b);

    return lhs;
}

static PyNumberMethods PyGLMatrix_tp_as_number =
{
	PyGLMatrix_add, //binaryfunc nb_add;
	PyGLMatrix_sub, //binaryfunc nb_subtract;
	PyGLMatrix_mul, //binaryfunc nb_multiply;
	0,		//binaryfunc nb_divide;
	0,		//binaryfunc nb_remainder;
	0,		//binaryfunc nb_divmod;
	0,		//ternaryfunc nb_power;
	0,		//unaryfunc nb_negative;
	0,		//unaryfunc nb_positive;
	0,		//unaryfunc nb_absolute;
	0,		//inquiry nb_nonzero;
	0,		//unaryfunc nb_invert;
	0,		//binaryfunc nb_lshift;
	0,		//binaryfunc nb_rshift;
	0,		//binaryfunc nb_and;
	0,		//binaryfunc nb_xor;
	0,		//binaryfunc nb_or;
	0,		//coercion nb_coerce;
	0,		//unaryfunc nb_int;
	0,		//unaryfunc nb_long;
	0,		//unaryfunc nb_float;
	0,		//unaryfunc nb_oct;
	0,		//unaryfunc nb_hex;
			/* Added in release 2.0 */
	PyGLMatrix_iadd,//binaryfunc nb_inplace_add;
	PyGLMatrix_isub,//binaryfunc nb_inplace_subtract;
	PyGLMatrix_imul,//binaryfunc nb_inplace_multiply;
	0,		//binaryfunc nb_inplace_divide;
	0,		//binaryfunc nb_inplace_remainder;
	0,		//ternaryfunc nb_inplace_power;
	0,		//binaryfunc nb_inplace_lshift;
	0,		//binaryfunc nb_inplace_rshift;
	0,		//binaryfunc nb_inplace_and;
	0,		//binaryfunc nb_inplace_xor;
	0,		//binaryfunc nb_inplace_or;

			/* Added in release 2.2 */
			/* The following require the Py_TPFLAGS_HAVE_CLASS flag */
	0,		//binaryfunc nb_floor_divide;
	0,		//binaryfunc nb_true_divide;
	0,		//binaryfunc nb_inplace_floor_divide;
	0		//binaryfunc nb_inplace_true_divide;
};

static PyTypeObject PyGLMatrix_t =
{
    PyObject_HEAD_INIT(NULL)    //!tp_head
    0,                          //!tp_internal
    "MathGL.GLMatrix",	        //!tp_name
    sizeof(PyGLMatrix),         //!tp_basicsize
    0,                          //!tp_itemsize
    PyGLMatrix_tp_dealloc,      //!tp_dealloc
    0,                          //!tp_print
    0,                          //!tp_getattr
    0,                          //!tp_setattr
    0,                          //!tp_compare
    0,                          //!tp_repr
    &PyGLMatrix_tp_as_number,   //!tp_as_number
    0,                          //!tp_as_sequence
    0,                          //!tp_as_mapping
    0,                          //!tp_hash
    0,                          //!tp_call
    0,                          //!tp_str
    PyObject_GenericGetAttr,    //!tp_getattro
    PyObject_GenericSetAttr,    //!tp_setattro
    0,                          //!tp_as_buffer
    Py_TPFLAGS_DEFAULT,
    "an OpenGL matrix",
    0,                          //!tp_traverse
    0,                          //!tp_clear
    0,                          //!tp_richcompare
    0,                          //!tp_weaklistoffset
    0,                          //!tp_iter
    0,                          //!tp_iternext
    PyGLMatrix_tp_methods,      //!tp_methods
    0,                          //!tp_members
    0,                          //!tp_getset
    0,                          //!tp_base
    0,                          //!tp_dict
    0,                          //!tp_descr_get
    0,                          //!tp_descr_set    
    0,                          //!tp_dictoffset
    PyGLMatrix_tp_init,         //!tp_init
    PyType_GenericAlloc,        //!tp_alloc
    PyType_GenericNew,          //!tp_new
    _PyObject_Del               //!tp_free
};

static PyObject* new_vector3(PyObject* self, PyObject* keys, PyObject* args);

//need to export the methods as well

static int PyGLVector3_tp_init(PyObject* self, PyObject* args, PyObject* kwds)
{
    return 0;
}

static void PyGLVector3_tp_dealloc(PyObject* self)
{
    PyGLVector3* vec = (PyGLVector3*)self;
    if(vec && vec->vector)
        delete vec->vector;
    if(self)
        self->ob_type->tp_free(self);
}

static PyMethodDef PyGLVector3_tp_methods[] =
{
    { NULL, NULL, NULL }
};

static PyObject* PyGLVector3_add(PyObject* lhs, PyObject* rhs)
{
    GLVector3d* a = ((PyGLVector3*)lhs)->vector;
    PyObject* ret = new_vector3(NULL,NULL,NULL);
    GLVector3d* retVector = ((PyGLVector3*)ret)->vector;

    GLVector3d* b = ((PyGLVector3*)rhs)->vector;
    *retVector = (*a) + (*b);

    return ret;
}

static PyObject* PyGLVector3_iadd(PyObject* lhs, PyObject* rhs)
{
    GLVector3d* a = ((PyGLVector3*)lhs)->vector;
    GLVector3d* b = ((PyGLVector3*)rhs)->vector;
    (*a) += (*b);

    return lhs;
}

static PyObject* PyGLVector3_sub(PyObject* lhs, PyObject* rhs)
{
    GLVector3d* a = ((PyGLVector3*)lhs)->vector;
    PyObject* ret = new_vector3(NULL,NULL,NULL);
    GLVector3d* retVector = ((PyGLVector3*)ret)->vector;

    GLVector3d* b = ((PyGLVector3*)rhs)->vector;
    *retVector = (*a) - (*b);

    return ret;
}

static PyObject* PyGLVector3_isub(PyObject* lhs, PyObject* rhs)
{
    GLVector3d* a = ((PyGLVector3*)lhs)->vector;
    GLVector3d* b = ((PyGLVector3*)rhs)->vector;
    (*a) -= (*b);

    return lhs;
}

static PyObject* PyGLVector3_mul(PyObject* lhs, PyObject* rhs)
{
    GLVector3d* a = ((PyGLVector3*)lhs)->vector;
    PyObject* ret = new_vector3(NULL,NULL,NULL);
    GLVector3d* retVector = ((PyGLVector3*)ret)->vector;

    double b = PyFloat_AS_DOUBLE(rhs);
    *retVector = (*a) * b;

    return ret;
}

static PyObject* PyGLVector3_imul(PyObject* lhs, PyObject* rhs)
{
    GLVector3d* a = ((PyGLVector3*)lhs)->vector;
    double b = PyFloat_AS_DOUBLE(rhs);
    (*a) *= b;

    return lhs;
}

static PyNumberMethods PyGLVector3_tp_as_number =
{
	PyGLVector3_add, //binaryfunc nb_add;
	PyGLVector3_sub, //binaryfunc nb_subtract;
	PyGLVector3_mul, //binaryfunc nb_multiply;
	0,		//binaryfunc nb_divide;
	0,		//binaryfunc nb_remainder;
	0,		//binaryfunc nb_divmod;
	0,		//ternaryfunc nb_power;
	0,		//unaryfunc nb_negative;
	0,		//unaryfunc nb_positive;
	0,		//unaryfunc nb_absolute;
	0,		//inquiry nb_nonzero;
	0,		//unaryfunc nb_invert;
	0,		//binaryfunc nb_lshift;
	0,		//binaryfunc nb_rshift;
	0,		//binaryfunc nb_and;
	0,		//binaryfunc nb_xor;
	0,		//binaryfunc nb_or;
	0,		//coercion nb_coerce;
	0,		//unaryfunc nb_int;
	0,		//unaryfunc nb_long;
	0,		//unaryfunc nb_float;
	0,		//unaryfunc nb_oct;
	0,		//unaryfunc nb_hex;
			/* Added in release 2.0 */
	PyGLVector3_iadd,//binaryfunc nb_inplace_add;
	PyGLVector3_isub,//binaryfunc nb_inplace_subtract;
	PyGLVector3_imul,//binaryfunc nb_inplace_multiply;
	0,		//binaryfunc nb_inplace_divide;
	0,		//binaryfunc nb_inplace_remainder;
	0,		//ternaryfunc nb_inplace_power;
	0,		//binaryfunc nb_inplace_lshift;
	0,		//binaryfunc nb_inplace_rshift;
	0,		//binaryfunc nb_inplace_and;
	0,		//binaryfunc nb_inplace_xor;
	0,		//binaryfunc nb_inplace_or;

			/* Added in release 2.2 */
			/* The following require the Py_TPFLAGS_HAVE_CLASS flag */
	0,		//binaryfunc nb_floor_divide;
	0,		//binaryfunc nb_true_divide;
	0,		//binaryfunc nb_inplace_floor_divide;
	0		//binaryfunc nb_inplace_true_divide;
};

static PyTypeObject PyGLVector3_t =
{
    PyObject_HEAD_INIT(NULL)    //!tp_head
    0,                          //!tp_internal
    "MathGL.GLVector3",	        //!tp_name
    sizeof(PyGLVector3),         //!tp_basicsize
    0,                          //!tp_itemsize
    PyGLVector3_tp_dealloc,      //!tp_dealloc
    0,                          //!tp_print
    0,                          //!tp_getattr
    0,                          //!tp_setattr
    0,                          //!tp_compare
    0,                          //!tp_repr
    &PyGLVector3_tp_as_number,   //!tp_as_number
    0,                          //!tp_as_sequence
    0,                          //!tp_as_mapping
    0,                          //!tp_hash
    0,                          //!tp_call
    0,                          //!tp_str
    PyObject_GenericGetAttr,    //!tp_getattro
    PyObject_GenericSetAttr,    //!tp_setattro
    0,                          //!tp_as_buffer
    Py_TPFLAGS_DEFAULT,
    "an OpenGL x,y,z vector",
    0,                          //!tp_traverse
    0,                          //!tp_clear
    0,                          //!tp_richcompare
    0,                          //!tp_weaklistoffset
    0,                          //!tp_iter
    0,                          //!tp_iternext
    PyGLVector3_tp_methods,      //!tp_methods
    0,                          //!tp_members
    0,                          //!tp_getset
    0,                          //!tp_base
    0,                          //!tp_dict
    0,                          //!tp_descr_get
    0,                          //!tp_descr_set    
    0,                          //!tp_dictoffset
    PyGLVector3_tp_init,         //!tp_init
    PyType_GenericAlloc,        //!tp_alloc
    PyType_GenericNew,          //!tp_new
    _PyObject_Del               //!tp_free
};

static PyObject* new_matrix(PyObject* self)
{
    PyGLMatrix* mat = PyObject_NEW(PyGLMatrix, &PyGLMatrix_t);
    
    mat->matrix = new GLMatrix4d();
    
    return (PyObject*)mat;
}

static PyObject* new_vector3(PyObject* self, PyObject* keys, PyObject* args)
{
    PyGLVector3* vec3 = PyObject_NEW(PyGLVector3, &PyGLVector3_t);
    
    vec3->vector = new GLVector3d();
    
    return (PyObject*)vec3;
}


static PyMethodDef pymathgl_funcs[] =
{
    { "GLMatrix", (PyCFunction)new_matrix, METH_NOARGS },
    { "GLVector3", (PyCFunction)new_vector3, METH_KEYWORDS },
    { NULL, NULL}
};


extern "C" void initMathGL()
{
    PyObject* module = Py_InitModule3("MathGL", pymathgl_funcs, "Python binding for MathGL++");
}

