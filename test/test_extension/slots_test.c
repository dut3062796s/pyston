#include <Python.h>
#include <stddef.h> /* For offsetof */

typedef struct {
    PyObject_HEAD;

    PyObject* dict;

    int n;
} slots_tester_object;

typedef struct {
    PyObject_HEAD;

    slots_tester_object* obj;

    int m;
} slots_tester_iterobj;

static PyObject *
slots_tester_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    slots_tester_object* obj;

    if (!_PyArg_NoKeywords("attrgetter()", kwds))
        return NULL;

    int n;
    if (!PyArg_ParseTuple(args, "i", &n))
        return NULL;

    printf("slots_tester_seq.__new__, %d\n", n);

    obj = (slots_tester_object*)type->tp_alloc(type, 0);
    if (obj == NULL)
        return NULL;

    obj->n = n - 1;

    return (PyObject *)obj;
}

static int
slots_tester_init(PyObject *self, PyObject *args, PyObject *kwds)
{
    printf("slots_tester_seq.__init__, %d\n", ((slots_tester_object*)self)->n);

    return 0;
}

static PyObject*
slots_tester_alloc(PyTypeObject* type, Py_ssize_t nitems) {
    printf("slots_tester_seq.tp_alloc, %s %ld\n", type->tp_name, nitems);

    return PyType_GenericAlloc(type, nitems);
}

static long slots_tester_seq_hash(slots_tester_object* obj) {
    printf("slots_tester_seq.__hash__\n");
    return obj->n ^ 1;
}

static PyObject* slots_tester_seq_richcmp(slots_tester_object* lhs, PyObject* rhs, int op) {
    printf("slots_tester_seq.richcmp(%d, %d)\n", lhs->n, op);

    Py_RETURN_TRUE;
    if (op % 2)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static PyObject *
slots_tester_seq_repr(slots_tester_object *obj)
{
    char buf[80];
    snprintf(buf, sizeof(buf), "<my custom repr: %d>", obj->n);
    return PyString_FromString(buf);
}

static PyObject *
slots_tester_seq_str(slots_tester_object *obj)
{
    char buf[80];
    snprintf(buf, sizeof(buf), "<my custom str: %d>", obj->n);
    return PyString_FromString(buf);
}

static PyObject *
slots_tester_seq_call(slots_tester_object *obj, PyObject *args, PyObject *kw)
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    return PyInt_FromLong(obj->n);
}

static PyObject*
slots_tester_seq_item(slots_tester_object *obj, Py_ssize_t i)
{
    if (i < 0 || i >= 5) {
        PyErr_SetString(PyExc_IndexError, "tuple index out of range");
        return NULL;
    }
    return PyInt_FromLong(i + obj->n);
}

PyDoc_STRVAR(slots_tester_seq_doc, "slots_tester_seq doc");

static PySequenceMethods slots_tester_seq_as_sequence = {
    (lenfunc)0,
    (binaryfunc)0,           /* sq_concat */
    (ssizeargfunc)0,         /* sq_repeat */
    (ssizeargfunc)slots_tester_seq_item,               /* sq_item */
    (ssizessizeargfunc)0,         /* sq_slice */
    0,                                          /* sq_ass_item */
    0,                                          /* sq_ass_slice */
    (objobjproc)0,             /* sq_contains */
};

static slots_tester_iterobj* slots_tester_iter(slots_tester_object *obj);

static PyTypeObject slots_tester_seq = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "slots_test.slots_tester_seq",            /* tp_name */
    sizeof(slots_tester_object),          /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    0,                                  /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_compare */
    (reprfunc)slots_tester_seq_repr,        /* tp_repr */
    0,                                  /* tp_as_number */
    &slots_tester_seq_as_sequence,          /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    (hashfunc)slots_tester_seq_hash, /* tp_hash */
    (ternaryfunc)slots_tester_seq_call,     /* tp_call */
    (reprfunc)slots_tester_seq_str,     /* tp_str */
    0,                                  /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                 /* tp_flags */
    slots_tester_seq_doc,                   /* tp_doc */
    0,                                  /* tp_traverse */
    0,                                  /* tp_clear */
    (richcmpfunc)slots_tester_seq_richcmp, /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    (getiterfunc)slots_tester_iter,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    0,                                  /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    slots_tester_init,                  /* tp_init */
    (allocfunc)slots_tester_alloc,                 /* tp_alloc */
    slots_tester_new,                   /* tp_new */
    0,                                  /* tp_free */
};

PyObject* slots_testeriter_iternext(slots_tester_iterobj* iter) {
    iter->m++;
    if (iter->m < iter->obj->n) {
        return PyInt_FromLong(iter->m);
    }
    return NULL;
}

static int
slots_testeriter_init(PyObject *self, PyObject *args, PyObject *kwds)
{
    printf("slots_testeriter_seq.__init__, %d\n", ((slots_tester_object*)self)->n);

    return 0;
}

static PyTypeObject slots_tester_seqiter = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "slots_test.slots_tester_seqiter",            /* tp_name */
    sizeof(slots_tester_iterobj),          /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    0,                                  /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_compare */
    (reprfunc)slots_tester_seq_repr,        /* tp_repr */
    0,                                  /* tp_as_number */
    0,          /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0, /* tp_hash */
    0,     /* tp_call */
    0,     /* tp_str */
    0,                                  /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                 /* tp_flags */
    0,                   /* tp_doc */
    0,                                  /* tp_traverse */
    0,                                  /* tp_clear */
    0, /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    (iternextfunc)slots_testeriter_iternext,                                  /* tp_iternext */
    0,                                  /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    slots_testeriter_init,                  /* tp_init */
    0,                 /* tp_alloc */
    0,                   /* tp_new */
    0,                                  /* tp_free */
};

static slots_tester_iterobj* slots_tester_iter(slots_tester_object *obj) {
    slots_tester_iterobj* rtn = PyObject_New(slots_tester_iterobj, &slots_tester_seqiter);
    Py_INCREF(obj);
    rtn->obj = obj;
    rtn->m = 0;
    return rtn;
}

static PyTypeObject slots_tester_nonsubclassable = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "slots_test.slots_tester_nonsubclassable",            /* tp_name */
    sizeof(slots_tester_object),          /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    0,                                  /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_compare */
    (reprfunc)slots_tester_seq_repr,        /* tp_repr */
    0,                                  /* tp_as_number */
    0,          /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0, /* tp_hash */
    0,     /* tp_call */
    0,     /* tp_str */
    0,                                  /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                 /* tp_flags */
    0,                   /* tp_doc */
    0,                                  /* tp_traverse */
    0,                                  /* tp_clear */
    0, /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    0,                                  /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    slots_tester_init,                  /* tp_init */
    0,                                  /* tp_alloc */
    slots_tester_new,                   /* tp_new */
    0,                                  /* tp_free */
};

static Py_ssize_t slots_tester_map_length(slots_tester_object* obj) {
    return obj->n;
}

static PyObject* slots_tester_map_subscript(slots_tester_object* obj, PyObject* key) {
    int n2 = PyInt_AsLong(key);
    return PyInt_FromLong(n2 + obj->n);
}

static int slots_tester_map_ass_sub(slots_tester_object* obj, PyObject* key, PyObject* value) {
    int n2 = PyInt_AsLong(key);
    printf("Assigning to subscript for object with n=%d, key=%d, set/delete=%d\n", obj->n, n2, !!value);

    return 0;
}

static PyMappingMethods slots_tester_map_asmapping = {
    (lenfunc)slots_tester_map_length,              /*mp_length*/
    (binaryfunc)slots_tester_map_subscript,        /*mp_subscript*/
    (objobjargproc)slots_tester_map_ass_sub,       /*mp_ass_subscript*/
};

static PyTypeObject slots_tester_map= {
    PyVarObject_HEAD_INIT(NULL, 0)
    "slots_test.slots_tester_map",            /* tp_name */
    sizeof(slots_tester_object),          /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    0,                                  /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_compare */
    (reprfunc)slots_tester_seq_repr,    /* tp_repr */
    0,                                  /* tp_as_number */
    0,          /* tp_as_sequence */
    &slots_tester_map_asmapping,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    0,     /* tp_call */
    0,                                  /* tp_str */
    0,                                  /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                 /* tp_flags */
    0,                   /* tp_doc */
    0,                                  /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    0,                                  /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    offsetof(slots_tester_object, dict), /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    slots_tester_new,                   /* tp_new */
    0,                                  /* tp_free */
};

#define _PYSTON_STRINGIFY(N) #N
#define PYSTON_STRINGIFY(N) _PYSTON_STRINGIFY(N)
#define CREATE_UN(N, R) \
static PyObject* N(slots_tester_object* lhs) { \
    printf(PYSTON_STRINGIFY(N) ", %d\n", lhs->n); \
    Py_INCREF(R); \
    return (PyObject*)R; \
}

#define CREATE_BIN(N) \
static PyObject* N(slots_tester_object* lhs, PyObject* rhs) { \
    printf(PYSTON_STRINGIFY(N) ", %d %s\n", lhs->n, Py_TYPE(rhs)->tp_name); \
    Py_INCREF(lhs); \
    return (PyObject*)lhs; \
}

CREATE_BIN(s_add);
CREATE_BIN(s_subtract);
CREATE_BIN(s_multiply);
CREATE_BIN(s_divide);
CREATE_BIN(s_remainder);
CREATE_BIN(s_divmod);
CREATE_UN(s_negative, lhs);
CREATE_UN(s_positive, lhs);
CREATE_UN(s_absolute, lhs);

static int s_nonzero(slots_tester_object* self) {
    printf("s_nonzero, %d\n", self->n);
    return self->n != 0;
}

CREATE_UN(s_invert, lhs);

static PyObject* s_power(slots_tester_object* lhs, PyObject* rhs, PyObject* mod) {
    printf("s_power, %d %s %s\n", lhs->n, Py_TYPE(rhs)->tp_name, Py_TYPE(mod)->tp_name);
    Py_INCREF(lhs);
    return (PyObject*)lhs;
}

CREATE_BIN(s_lshift);
CREATE_BIN(s_rshift);
CREATE_BIN(s_and);
CREATE_BIN(s_xor);
CREATE_BIN(s_or);

CREATE_UN(s_int, Py_True);
CREATE_UN(s_long, Py_True);
CREATE_UN(s_float, PyFloat_FromDouble(1.0));
CREATE_UN(s_oct, PyString_FromString("oct"));
CREATE_UN(s_hex, PyString_FromString("hex"));

#undef CREATE_BIN

static PyNumberMethods slots_tester_as_number = {
    (binaryfunc)s_add,                                  /* nb_add */
    (binaryfunc)s_subtract,                             /* nb_subtract */
    (binaryfunc)s_multiply,                             /* nb_multiply */
    (binaryfunc)s_divide,                               /* nb_divide */
    (binaryfunc)s_remainder,                                          /* nb_remainder */
    (binaryfunc)s_divmod,                                          /* nb_divmod */
    (ternaryfunc)s_power,                                          /* nb_power */
    (unaryfunc)s_negative,                  /* nb_negative */
    (unaryfunc)s_positive,                  /* nb_positive */
    (unaryfunc)s_absolute,                       /* nb_absolute */
    (inquiry)s_nonzero,                     /* nb_nonzero */
    (unaryfunc)s_invert,                                          /*nb_invert*/
    (binaryfunc)s_lshift,                                          /*nb_lshift*/
    (binaryfunc)s_rshift,                                          /*nb_rshift*/
    (binaryfunc)s_and,                                          /*nb_and*/
    (binaryfunc)s_xor,                                          /*nb_xor*/
    (binaryfunc)s_or,                                          /*nb_or*/
    0,                                          /*nb_coerce*/
    (unaryfunc)s_int,                                          /*nb_int*/
    (unaryfunc)s_long,                                          /*nb_long*/
    (unaryfunc)s_float,                                          /*nb_float*/
    (unaryfunc)s_oct,                                          /*nb_oct*/
    (unaryfunc)s_hex,                                          /*nb_hex*/
    0,                                          /*nb_inplace_add*/
    0,                                          /*nb_inplace_subtract*/
    0,                                          /*nb_inplace_multiply*/
    0,                                          /*nb_inplace_divide*/
    0,                                          /*nb_inplace_remainder*/
    0,                                          /*nb_inplace_power*/
    0,                                          /*nb_inplace_lshift*/
    0,                                          /*nb_inplace_rshift*/
    0,                                          /*nb_inplace_and*/
    0,                                          /*nb_inplace_xor*/
    0,                                          /*nb_inplace_or*/
    0,                               /* nb_floor_divide */
    0,                                          /* nb_true_divide */
    0,                                          /* nb_inplace_floor_divide */
    0,                                          /* nb_inplace_true_divide */
};

static PyTypeObject slots_tester_num = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "slots_test.slots_tester_num",            /* tp_name */
    sizeof(slots_tester_object),          /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    0,                                  /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_compare */
    0,        /* tp_repr */
    &slots_tester_as_number,                                  /* tp_as_number */
    0,          /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    0,     /* tp_call */
    (reprfunc)slots_tester_seq_str,     /* tp_str */
    0,                                  /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,                 /* tp_flags */
    0,                   /* tp_doc */
    0,                                  /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    0,                                  /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    slots_tester_new,                   /* tp_new */
    0,                                  /* tp_free */
};

typedef struct {
    slots_tester_object base;

    int n2;
} slots_tester_object_sub;

static PyTypeObject slots_tester_sub = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "slots_test.slots_tester_sub",            /* tp_name */
    sizeof(slots_tester_object_sub),          /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    0,                                  /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_compare */
    0,        /* tp_repr */
    0,                                  /* tp_as_number */
    0,          /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    0,     /* tp_call */
    0,     /* tp_str */
    0,                                  /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,                 /* tp_flags */
    0,                   /* tp_doc */
    0,                                  /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    0,                                  /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    &slots_tester_seq,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    0,                   /* tp_new */
    0,                                  /* tp_free */
};

static PyObject*
getattr_returnnull(PyObject* self, const char* attr)
{
  return NULL;
}

static PyTypeObject slots_tester_nullreturngetattr = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "slots_test.slots_tester_nullreturngetattr",            /* tp_name */
    sizeof(slots_tester_object_sub),          /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    0,                                  /* tp_dealloc */
    0,                                  /* tp_print */
    (getattrfunc)getattr_returnnull,    /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_compare */
    0,        /* tp_repr */
    0,                                  /* tp_as_number */
    0,          /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    0,     /* tp_call */
    0,     /* tp_str */
    0,                                  /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES,                 /* tp_flags */
    0,                   /* tp_doc */
    0,                                  /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    0,                                  /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    &slots_tester_seq,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    0,                   /* tp_new */
    0,                                  /* tp_free */
};

// Tests the correctness of the CAPI slots when the attributes get set in Python code:
static PyObject *
call_funcs(PyObject* _module, PyObject* args) {
    PyObject* obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;

    printf("\n");

    PyTypeObject* cls = Py_TYPE(obj);
    printf("Received a %s object\n", cls->tp_name);

    if (cls->tp_repr) {
        PyObject* rtn = cls->tp_repr(obj);
        printf("tp_repr exists and returned: '%s'\n", PyString_AsString(rtn));
        Py_DECREF(rtn);
    }

    if (cls->tp_new) {
        PyObject* rtn = cls->tp_new(cls, PyTuple_New(0), PyDict_New());
        if (!rtn) {
            printf("tp_new_exists but returned an error!\n");
            PyErr_Print();
        } else {
            printf("tp_new exists and returned an object of type: '%s'\n", Py_TYPE(rtn)->tp_name);
            Py_DECREF(rtn);
        }
    }

    if (cls->tp_new) {
        printf("tp_new exists\n");
    } else {
        printf("tp_new doesnt exist\n");
    }

    if (cls->tp_init) {
        printf("tp_init exists\n");
    } else {
        printf("tp_init doesnt exist\n");
    }

    if (cls->tp_call) {
        printf("tp_call exists\n");
    } else {
        printf("tp_call doesnt exist\n");
    }

    if (cls->tp_getattr) {
        printf("tp_getattr exists\n");
    } else {
        printf("tp_getattr doesnt exist\n");
    }

    // we aren't checking for tp_getattro.  it's set in cpython and not in pyston

    if (cls->tp_setattr) {
        printf("tp_setattr exists\n");
    } else {
        printf("tp_setattr doesnt exist\n");
    }

    // we aren't checking for tp_setattro.  it's set in cpython and not in pyston

    if (cls->tp_descr_get) {
        printf("tp_descr_get exists\n");
    } else {
        printf("tp_descr_get doesnt exist\n");
    }

    if (cls->tp_as_mapping) {
        printf("tp_as_mapping exists\n");
        PyMappingMethods* map = cls->tp_as_mapping;

        if (map->mp_subscript) {
            PyObject* rtn = map->mp_subscript(obj, PyInt_FromLong(1));
            printf("mp_subscript exists and returned\n");
            Py_DECREF(rtn);
        } else {
            printf("mp_subscript does not exist\n");
        }

        if (map->mp_length) {
            Py_ssize_t rtn = map->mp_length(obj);
            printf("mp_length exists and returned %ld\n", rtn);
        }
    } else {
        printf("tp_as_mapping doesnt exist\n");
    }

    if (cls->tp_as_sequence) {
        printf("tp_as_sequence exists\n");
        PySequenceMethods* seq = cls->tp_as_sequence;

        if (seq->sq_length) {
            Py_ssize_t rtn = seq->sq_length(obj);
            printf("sq_length exists and returned %ld\n", rtn);
        }

        if (seq->sq_item) {
            PyObject* rtn = cls->tp_as_sequence->sq_item(obj, 1);
            printf("sq_item exists and returned\n");
            Py_DECREF(rtn);
        }
    } else {
        printf("tp_as_sequence doesnt exist\n");
    }

    if (cls->tp_as_number) {
        printf("tp_as_number exists\n");
        PyNumberMethods* num = cls->tp_as_number;

        if (!(cls->tp_flags & Py_TPFLAGS_CHECKTYPES)) {
            printf("CHECKTYPES is not set!\n");
        }

#define CHECK_UN(N) \
        if (num->N) { \
            PyObject* res = num->N(obj); \
            printf(PYSTON_STRINGIFY(N) " exists and returned a %s\n", Py_TYPE(res)->tp_name); \
            Py_DECREF(res);  \
        }

#define CHECK_BIN(N) \
        if (num->N) { \
            PyObject* res = num->N(obj, obj); \
            printf(PYSTON_STRINGIFY(N) " exists and returned a %s\n", Py_TYPE(res)->tp_name); \
            Py_DECREF(res);  \
        }

        CHECK_BIN(nb_add);
        CHECK_BIN(nb_subtract);
        CHECK_BIN(nb_multiply);
        CHECK_BIN(nb_divide);
        CHECK_BIN(nb_remainder);
        CHECK_BIN(nb_divmod);

        CHECK_UN(nb_negative);
        CHECK_UN(nb_positive);
        CHECK_UN(nb_absolute);

        if (num->nb_nonzero) {
            int n = num->nb_nonzero(obj);
            printf("nb_nonzero exists and returned %d\n", n);
        }

        CHECK_UN(nb_invert);

        if (num->nb_power) {
            PyObject* res = num->nb_power(obj, obj, Py_None);
            printf("nb_power exists and returned a %s\n", Py_TYPE(res)->tp_name);
            Py_DECREF(res);
        }

        CHECK_BIN(nb_lshift);
        CHECK_BIN(nb_rshift);
        CHECK_BIN(nb_and);
        CHECK_BIN(nb_xor);
        CHECK_BIN(nb_or);

        CHECK_UN(nb_int);
        CHECK_UN(nb_long);
        CHECK_UN(nb_float);
        CHECK_UN(nb_oct);
        CHECK_UN(nb_hex);
#undef CHECK_BIN

    } else {
        printf("tp_as_number doesnt exist\n");
    }

    Py_RETURN_NONE;
}

static PyMethodDef SlotsMethods[] = {
    {"call_funcs", call_funcs, METH_VARARGS, "Call slotted functions."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initslots_test(void)
{
    PyObject *m;

    m = Py_InitModule("slots_test", SlotsMethods);
    if (m == NULL)
        return;

    int res = PyType_Ready(&slots_tester_seq);
    if (res < 0)
        return;

    res = PyType_Ready(&slots_tester_seqiter);
    if (res < 0)
        return;

    res = PyType_Ready(&slots_tester_map);
    if (res < 0)
        return;

    res = PyType_Ready(&slots_tester_num);
    if (res < 0)
        return;

    res = PyType_Ready(&slots_tester_sub);
    if (res < 0)
        return;

    res = PyType_Ready(&slots_tester_nonsubclassable);
    if (res < 0)
        return;

    res = PyType_Ready(&slots_tester_nullreturngetattr);
    if (res < 0)
        return;

    // Not sure if the result of PyInt_FromLong needs to be decref'd
    PyDict_SetItemString(slots_tester_seq.tp_dict, "set_through_tpdict", PyInt_FromLong(123));

    PyModule_AddObject(m, "SlotsTesterSeq", (PyObject *)&slots_tester_seq);
    PyModule_AddObject(m, "SlotsTesterMap", (PyObject *)&slots_tester_map);
    PyModule_AddObject(m, "SlotsTesterNum", (PyObject *)&slots_tester_num);
    PyModule_AddObject(m, "SlotsTesterSub", (PyObject *)&slots_tester_sub);
    PyModule_AddObject(m, "SlotsTesterNonsubclassable", (PyObject *)&slots_tester_nonsubclassable);
    PyModule_AddObject(m, "SlotsTesterNullReturnGetAttr", (PyObject *)&slots_tester_nullreturngetattr);
}
