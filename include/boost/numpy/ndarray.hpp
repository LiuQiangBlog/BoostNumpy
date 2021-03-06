/**
 * $Id$
 *
 * Copyright (C)
 * 2013 - $Date$
 *     Martin Wolf <boostnumpy@martin-wolf.org>
 * 2010-2012
 *     Jim Bosch
 *
 * @file    boost/numpy/ndarray.hpp
 * @version $Revision$
 * @date    $Date$
 * @author  Martin Wolf <boostnumpy@martin-wolf.org>,
 *          Jim Bosch
 *
 * @brief This file defines the boost::numpy::ndarray object manager and some
 *        utility functions.
 *
 *        This file is distributed under the Boost Software License,
 *        Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *        http://www.boost.org/LICENSE_1_0.txt).
 */
#ifndef BOOST_NUMPY_NDARRAY_HPP_INCLUDED
#define BOOST_NUMPY_NDARRAY_HPP_INCLUDED

#include <stdint.h>

#include <cstring>
#include <vector>

#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_same.hpp>

#include <boost/python.hpp>

#include <boost/numpy/numpy_c_api.hpp>
#include <boost/numpy/dtype.hpp>
#include <boost/numpy/object_manager_traits.hpp>

namespace boost {
namespace numpy {

/**
 *  @brief A boost.python "object manager" (subclass of object) for
 *         numpy.ndarray.
 *
 *  @todo This could have a lot more functionality
 *        (like boost::python::numeric::array).
 *        Right now all that exists is what was needed to move raw data between
 *        C++ and Python.
 */
class ndarray : public python::object
{
  public:
    /**
     *  @brief Enum to represent the numpy's internal array flags.
     */
    enum flags
    {
        NONE = 0x0,

        // Flags an array can have.
        C_CONTIGUOUS = NPY_ARRAY_C_CONTIGUOUS,
        F_CONTIGUOUS = NPY_ARRAY_F_CONTIGUOUS,
        V_CONTIGUOUS = NPY_ARRAY_C_CONTIGUOUS | NPY_ARRAY_F_CONTIGUOUS,
        ALIGNED      = NPY_ARRAY_ALIGNED,
        OWNDATA      = NPY_ARRAY_OWNDATA,
        WRITEABLE    = NPY_ARRAY_WRITEABLE,
        UPDATEIFCOPY = NPY_ARRAY_UPDATEIFCOPY,
        BEHAVED      = NPY_ARRAY_BEHAVED,    // ALIGNED | WRITEABLE
        CARRAY       = NPY_ARRAY_CARRAY,     // C_CONTIGUOUS | ALIGNED | WRITEABLE
        CARRAY_RO    = NPY_ARRAY_CARRAY_RO,  // C_CONTIGUOUS | ALIGNED
        FARRAY       = NPY_ARRAY_FARRAY,     // F_CONTIGUOUS | ALIGNED | WRITEABLE
        FARRAY_RO    = NPY_ARRAY_FARRAY_RO,  // F_CONTIGUOUS | ALIGNED
        DEFAULT      = NPY_ARRAY_DEFAULT,    // CARRAY
        UPDATE_ALL   = NPY_ARRAY_UPDATE_ALL, // C_CONTIGUOUS | F_CONTIGUOUS | ALIGNED

        // Requirements for new created arrays.
        FORCECAST   = NPY_ARRAY_FORCECAST,
        ENSURECOPY  = NPY_ARRAY_ENSURECOPY,
        ENSUREARRAY = NPY_ARRAY_ENSUREARRAY,
        NOTSWAPPED  = NPY_ARRAY_NOTSWAPPED,
        BEHAVED_NS  = NPY_ARRAY_BEHAVED_NS // ALIGNED | WRITEABLE | NOTSWAPPED
    };

    BOOST_PYTHON_FORWARD_OBJECT_CONSTRUCTORS(ndarray, python::object);

    //__________________________________________________________________________
    /**
     * @brief Copies the elements of the array into a std::vector. The
     *        dtype constructed from the value_type must be equivalent to the
     *        dtype of the array.
     */
    template<typename T>
    std::vector<T>
    as_vector() const;

    //__________________________________________________________________________
    /**
     * @brief Returns a view of the array with the given dtype.
     */
    ndarray
    view(dtype const & dt) const;

    //__________________________________________________________________________
    /**
     * @brief Flattened copy of the array.
     */
    ndarray
    flatten(std::string const & order="C") const;

    //__________________________________________________________________________
    /**
     * @brief Makes a copy of the array. The result
     *     array will have the given order ("C", "F", "A", "K").
     *
     * @note In cases where the ndarray is an object array, the objects are
     *     only copied when they are atomic Python types. Otherwise the
     *     objects are not copied.
     *
     * @internal It calls the Python method "copy" of numpy's ndarray class.
     */
    ndarray
    copy(std::string const & order="C") const;

    //__________________________________________________________________________
    /**
     * @brief Makes a deep copy of the array. The result
     *     array will have the given order ("C", "F", "A", "K").
     *     If the array is an object array, the objects are also copied using
     *     the copy.deepcopy function.
     *
     * @internal It calls copy() first and then copies each element if the
     *     array is an object array.
     */
    ndarray
    deepcopy(std::string const & order="C") const;

    /**
     * @brief Checks if the given flags are set for this ndarray.
     */
    bool
    check_flags(flags const flags) const;

    //__________________________________________________________________________
    /**
     * @brief Clears (disables) the given flags of this ndarray object.
     * @internal It calls the PyArray_CLEARFLAGS function.
     */
    void
    clear_flags(flags const flags);

    //__________________________________________________________________________
    /**
     * @brief Enables the given flags of this ndarray object.
     * @internal It calls the PyArray_ENABLEFLAGS function.
     */
    void
    enable_flags(flags const flags);

    //__________________________________________________________________________
    /**
     * @brief Returns the object that owns the array's data, or None if the
     *        array owns its own data.
     */
    python::object
    get_base() const;

    //__________________________________________________________________________
    /**
     * @brief Sets the base object (i.e. the owner of the data).
     * @note This function increases the reference count of the given base
     *       object. But the reference count of the already set base object is
     *       left untouched!
     */
    void
    set_base(object_cref base);

    //__________________________________________________________________________
    /**
     * @brief Return the array's data-type descriptor object.
     */
    dtype
    get_dtype() const;

    //__________________________________________________________________________
    /**
     * @brief Return the array flags.
     */
    flags const
    get_flags() const;

    //__________________________________________________________________________
    /**
     * @brief Return the number of array dimensions.
     */
    int
    get_nd() const;

    //__________________________________________________________________________
    /**
     * @brief Returns the total number of elements of the array.
     */
    intptr_t
    get_size() const;

    //__________________________________________________________________________
    /**
     * @brief Return the shape of the array as a C array of intptr_t values
     *        (length == get_nd()).
     */
    intptr_t const *
    get_shape() const;

    //__________________________________________________________________________
    /**
     * @brief Returns the shape of the array as a std::vector<intptr_t> object
     *        with a length equal to get_nd().
     */
    inline
    std::vector<intptr_t>
    get_shape_vector() const
    {
        int const nd = this->get_nd();
        intptr_t const * shape = this->get_shape();
        std::vector<intptr_t> shape_vec(nd);
        for(int i=0; i<nd; ++i) {
            shape_vec[i] = shape[i];
        }
        return shape_vec;
    }

    //__________________________________________________________________________
    /**
     * @brief Return the size of the n'th dimension.
     */
    inline
    intptr_t const
    shape(int n) const
    {
        return get_shape()[n];
    }

    //__________________________________________________________________________
    /**
     * @brief Returns the strides of the array as an array of integers
     *        (length == get_nd()).
     */
    intptr_t const *
    get_strides() const;

    //__________________________________________________________________________
    /**
     * \brief Returns the strides of the array as a std::vector<intptr_t>
     *        object.
     */
    std::vector<intptr_t>
    get_strides_vector() const
    {
        int const nd = this->get_nd();
        intptr_t const * strides = this->get_strides();
        std::vector<intptr_t> strides_vec(nd);
        memcpy(&strides_vec.front(), strides, nd*sizeof(intptr_t));
        return strides_vec;
    }

    //__________________________________________________________________________
    /**
     * @brief Return the stride of the nth dimension.
     */
    int const
    strides(int n) const
    {
        return get_strides()[n];
    }

    //__________________________________________________________________________
    /**
     * @brief Return the array's raw data pointer.
     *
     *  This returns char so stride math works properly on it.
     *  It's pretty much expected that the user will have to reinterpret_cast
     *  it.
     */
    char *
    get_data() const;

    //__________________________________________________________________________
    /**
     * @brief Checks if the array is an object array, i.e. holding pointers to
     *     (arbitrary) Python objects. Returns true if it as an object array,
     *     and false otherwise.
     */
    bool
    is_object_array() const;

    //__________________________________________________________________________
    /**
     * @brief Reverse the dimensions of the array.
     */
    ndarray
    transpose() const;

    //__________________________________________________________________________
    /**
     * @brief Eliminate any unit-sized dimensions.
     */
    ndarray
    squeeze() const;

    //__________________________________________________________________________
    /**
     * @brief Equivalent to self.reshape(shape) in Python.
     */
    ndarray
    reshape(python::tuple const & shape) const;
    //--------------------------------------------------------------------------
    ndarray
    reshape(python::list const & shape) const;
    //--------------------------------------------------------------------------
    ndarray
    reshape(std::vector<intptr_t> const & shape) const;

    //__________________________________________________________________________
    /**
     *  @brief If the array contains only a single element, i.e. it has shape
     *      (1,), or it is a 0-dimensional array, return it as an
     *      array scalar; otherwise return the array.
     */
    python::object
    scalarize() const;

    //__________________________________________________________________________
    /**
     * @brief Checks if the ndarray object has the given shape.
     */
    bool
    has_shape(std::vector<intptr_t> const & shape) const;

    //__________________________________________________________________________
    /**
     * @brief Copy assignment operator for a boost::python::object object.
     *
     * @note This does not ensure, that the given python object is of sub-type
     *       PyArray_Type. It just calls the assignment operator of the base
     *       class, i.e. boost::python::object which will just set the object
     *       pointers and takes care of the Python reference count of the two
     *       objects.
     */
    inline
    ndarray &
    operator=(boost::python::object & rhs);

    //__________________________________________________________________________
    /**
     * @brief Calls __getitem__ with the given object as key. The key can be of
     *        any type, that is convertible to the boost::python::object type or
     *        that is derived from boost::python::object.
     *        The returned object should either be of type boost::python::object
     *        or boost::numpy::ndarray. In the former case the
     *        returned object could be an array scalar or a ndarray.
     *
     *        get_item provides all possible kinds of array indexing supported
     *        by numpy.
     */
    //--- R = boost::python::object, T = derived from boost::python::object
    template <typename R, typename T>
    typename enable_if< boost::mpl::and_< is_same<R, python::object>, is_base_of<python::object, T> >, python::object>::type
    get_item(T const & obj) const;

    //--- R = boost::python::object, T = not derived from boost::python::object
    template <typename R, typename T>
    typename enable_if< boost::mpl::and_< is_same<R, python::object>, boost::mpl::not_< is_base_of<python::object, T> > >, python::object>::type
    get_item(T const & obj) const;

    //--- R = ndarray, T = derived from boost::python::object
    template <typename R, typename T>
    typename enable_if< boost::mpl::and_< is_same<R, ndarray>, is_base_of<python::object, T> >, ndarray>::type
    get_item(T const & obj) const;

    //--- R = ndarray, T = not derived from boost::python::object
    template <typename R, typename T>
    typename enable_if< boost::mpl::and_< is_same<R, ndarray>, boost::mpl::not_< is_base_of<python::object, T> > >, ndarray>::type
    get_item(T const & obj) const;

    //__________________________________________________________________________
    /**
     * @brief Access operator for retrieving an item from the ndarray.
     *        The returned object is always of type ndarray.
     */
    template <typename T>
    inline
    ndarray
    operator[](T const & obj) const;

  protected:
    python::object
    get_bpo_item(python::object const & obj) const;
};

//______________________________________________________________________________
/**
 *  @brief Construct a new array with the given shape and data type, with data
 *         initialized to zero.
 */
ndarray zeros(int nd, intptr_t const * shape, dtype const & dt);
ndarray zeros(python::tuple const & shape, dtype const & dt);
ndarray zeros(std::vector<intptr_t> const & shape, dtype const & dt);

/**
 *  @brief Construct a new array with the given shape and data type, with data
 *         left uninitialized.
 */
ndarray empty(int nd, intptr_t const * shape, dtype const & dt);
ndarray empty(python::tuple const & shape, dtype const & dt);
ndarray empty(std::vector<intptr_t> const & shape, dtype const & dt);

//______________________________________________________________________________
/**
 *  @brief Construct a new array from an arbitrary Python sequence.
 *
 *  @todo This doesn't seem to handle ndarray subtypes the same way that
 *        "numpy.array" does in Python.
 */
ndarray array(python::object const & obj);
ndarray array(python::object const & obj, dtype const & dt);

//==============================================================================
namespace detail {

//______________________________________________________________________________
ndarray
from_data_impl(
    void *                           data
  , dtype const &                    dt
  , std::vector<Py_intptr_t> const & shape
  , std::vector<Py_intptr_t> const & strides
  , python::object const *           owner
  , bool                             writeable
  , bool                             set_owndata_flag
);

//______________________________________________________________________________
template <typename Container>
ndarray
from_data_impl(
    void *                 data
  , dtype const &          dt
  , Container const &      shape
  , Container const &      strides
  , python::object const * owner
  , bool                   writeable
  , bool                   set_owndata_flag
  , typename boost::enable_if< boost::mpl::and_< boost::mpl::not_< boost::is_same<typename Container::value_type, Py_intptr_t> >
                                               , boost::is_integral<typename Container::value_type>
                                               >
                             >::type* enabled = NULL
)
{
    std::vector<Py_intptr_t> shape_(shape.begin(), shape.end());
    std::vector<Py_intptr_t> strides_(strides.begin(), strides.end());
    return from_data_impl(data, dt, shape_, strides_, owner, writeable, set_owndata_flag);
}

//______________________________________________________________________________
ndarray
from_data_impl(
    void *                 data
  , dtype const &          dt
  , python::object const & shape
  , python::object const & strides
  , python::object const * owner
  , bool                   writeable
  , bool                   set_owndata_flag
);

}/*detail*/
//==============================================================================

//______________________________________________________________________________
/**
 *  @brief Construct a new writeable ndarray object from a raw pointer to
 *         existing data.
 *
 *  @param[in] data    Raw pointer to the first element of the array.
 *  @param[in] dt      Data type descriptor.  Often retrieved with
 *                     dtype::get_builtin().
 *  @param[in] shape   Shape of the array as STL container of integers; must
 *                     have begin() and end().
 *  @param[in] strides Strides of the array as STL container of integers; must
 *                     have begin() and end().
 *  @param[in] owner   An arbitray Python object that owns the data the data
 *                     pointer points to. The array object will keep a reference
 *                     to the object, and decrement it's reference count when
 *                     the array goes out of scope. If set to python::object(),
 *                     the array will own the data and the array's base object
 *                     will be set to NULL.
 *  @param[in] set_owndata_flag Sometimes it is necessary to set the owner
 *                     later. If set_owndata_flag is set to false and no owner
 *                     is specified, the created array will not own the data,
 *                     and the data will not be deallocated when the ndarray
 *                     is destroyed.
 */
template <typename Container>
inline
ndarray
from_data(
    void *                 data
  , dtype const &          dt
  , Container const &      shape
  , Container const &      strides
  , python::object const * owner = NULL
  , bool                   set_owndata_flag = true
)
{
    return detail::from_data_impl(data, dt, shape, strides, owner, true, set_owndata_flag);
}

//______________________________________________________________________________
/**
 *  @brief Construct a new read-only ndarray object from a const void raw
 *         pointer to constant data.
 *
 *  @param[in] data    Raw pointer to the first element of the constant data.
 *  @param[in] dt      Data type descriptor. Often retrieved with
 *                     dtype::get_builtin().
 *  @param[in] shape   Shape of the array as STL container of integers; must
 *                     have begin() and end().
 *  @param[in] strides Strides of the array as STL container of integers; must
 *                     have begin() and end().
 *  @param[in] owner   An arbitray Python object that owns the data the data
 *                     pointer points to. The array object will keep a reference
 *                     to the object, and decrement it's reference count when
 *                     the array goes out of scope. If set to python::object(),
 *                     the array will own the data and the array's base object
 *                     will be set to NULL.
 *  @param[in] set_owndata_flag Sometimes it is necessary to set the owner
 *                     later. If set_owndata_flag is set to false and no owner
 *                     is specified, the created array will not own the data,
 *                     and the data will not be deallocated when the ndarray
 *                     is destroyed.
 */
template <typename Container>
inline
ndarray
from_data(
    void const *           data
  , dtype const &          dt
  , Container const &      shape
  , Container const &      strides
  , python::object const * owner = NULL
  , bool                   set_owndata_flag = true
)
{
    return detail::from_data_impl(const_cast<void*>(data), dt, shape, strides, owner, false, set_owndata_flag);
}

//______________________________________________________________________________
/**
 *  @brief Transform an arbitrary boost::python::object into a numpy array
 *         having the given data type and the given requirements.
 *
 *  @param[in] obj     An arbitrary boost::python::object to convert.
 *                     Arrays that meet the requirements will be passed through
 *                     directly.
 *  @param[in] dt      Data type descriptor. Often retrieved with
 *                     dtype::get_builtin().
 *  @param[in] nd_min  Minimum number of dimensions.
 *  @param[in] nd_max  Maximum number of dimensions.
 *  @param[in] flags   Bitwise OR of flags specifying additional requirements.
 *
 *  @internal It uses the PyArray_FromAny macro.
 */
ndarray
from_object(
    python::object const & obj,
    dtype const &          dt,
    int                    nd_min,
    int                    nd_max,
    ndarray::flags         flags = ndarray::NONE);

//______________________________________________________________________________
inline
ndarray
from_object(
    python::object const & obj,
    dtype const &          dt,
    int                    nd,
    ndarray::flags         flags = ndarray::NONE)
{
    return from_object(obj, dt, nd, nd, flags);
}

//______________________________________________________________________________
inline
ndarray
from_object(
    python::object const & obj,
    dtype const &          dt,
    ndarray::flags         flags = ndarray::NONE)
{
    return from_object(obj, dt, 0, 0, flags);
}

//______________________________________________________________________________
/**
 *  @brief Transform an arbitrary boost::python::object into a numpy array
 *         fulfilling the given requirements and having a data type that fits
 *         best.
 *
 *  @param[in] obj     An arbitrary boost::python::object to convert.
 *                     Arrays that meet the requirements will be passed through
 *                     directly.
 *  @param[in] nd_min  Minimum number of dimensions.
 *                     If set to 0, it will be ignored.
 *  @param[in] nd_max  Maximum number of dimensions.
 *                     If set to 0, it will be ignored.
 *  @param[in] flags   Bitwise OR of flags specifying additional requirements.
 *
 *  @internal It uses the PyArray_FromAny macro with dtype set to NULL.
 */
ndarray
from_object(
    python::object const & obj,
    int                    nd_min,
    int                    nd_max,
    ndarray::flags         flags = ndarray::NONE);

//______________________________________________________________________________
inline
ndarray from_object(
    python::object const & obj,
    int                    nd,
    ndarray::flags         flags = ndarray::NONE)
{
    return from_object(obj, nd, nd, flags);
}

//______________________________________________________________________________
inline
ndarray
from_object(
    python::object const & obj,
    ndarray::flags         flags = ndarray::NONE)
{
    return from_object(obj, 0, 0, flags);
}

//______________________________________________________________________________
inline
ndarray::flags
operator|(ndarray::flags a, ndarray::flags b)
{
    return ndarray::flags(int(a) | int(b));
}

inline
ndarray::flags
operator&(ndarray::flags a, ndarray::flags b)
{
    return ndarray::flags(int(a) & int(b));
}

//______________________________________________________________________________
template<typename T>
std::vector<T>
ndarray::
as_vector() const
{
    // Check if the dtype of the given element type is equivalent to the
    // dtype of this array.
    dtype dt = dtype::get_builtin<T>();
    if(! dtype::equivalent(get_dtype(), dt))
    {
        PyErr_SetString(PyExc_TypeError,
            "The given type is not equivalent to the array data type!");
        python::throw_error_already_set();
    }

    // Check if the ndarray is contigious in memory.
    if(! (check_flags(C_CONTIGUOUS) || check_flags(F_CONTIGUOUS)))
    {
        PyErr_SetString(PyExc_AssertionError,
            "The ndarray must be stored as a C or Fortran contiguous array in "
            "memory.");
        python::throw_error_already_set();
    }

    const intptr_t N = get_size();

    std::vector<T> vec(
        reinterpret_cast<T *>(get_data())
      , reinterpret_cast<T *>(get_data()) + N
    );
    return vec;
}

//______________________________________________________________________________
template <typename R, typename T>
typename enable_if< boost::mpl::and_< is_same<R, python::object>, is_base_of<python::object, T> >, python::object>::type
ndarray::
get_item(T const & obj) const
{
    return this->get_bpo_item(obj);
}

template <typename R, typename T>
typename enable_if< boost::mpl::and_< is_same<R, python::object>, boost::mpl::not_< is_base_of<python::object, T> > >, python::object>::type
ndarray::
get_item(T const & obj) const
{
    python::object bpo(obj);
    return this->get_bpo_item(bpo);
}

template <typename R, typename T>
typename enable_if< boost::mpl::and_< is_same<R, ndarray>, is_base_of<python::object, T> >, ndarray>::type
ndarray::
get_item(T const & obj) const
{
    python::object item = this->get_bpo_item(obj);
    ndarray arr = from_object(item);
    return arr;
}

template <typename R, typename T>
typename enable_if< boost::mpl::and_< is_same<R, ndarray>, boost::mpl::not_< is_base_of<python::object, T> > >, ndarray>::type
ndarray::
get_item(T const & obj) const
{
    python::object bpo(obj);
    python::object item = this->get_bpo_item(bpo);
    ndarray arr = from_object(item);
    return arr;
}

//______________________________________________________________________________
template <typename T>
inline
ndarray
ndarray::
operator[](T const & obj) const
{
    return this->get_item<ndarray, T>(obj);
}

//______________________________________________________________________________
inline
ndarray &
ndarray::
operator=(boost::python::object & rhs)
{
    python::object::operator=(rhs);
    return *this;
}

//______________________________________________________________________________
/**
 * \brief Checks if the given boost::python::object object is any scalar value
 *     (either a Python scalar or a numpy array scalar (i.e. nd=0).
 * \internal Calls PyArray_IsAnyScalar.
 */
bool
is_any_scalar(python::object const & obj);

//______________________________________________________________________________
/**
 * \brief Checks of the given boost::python::object object is either an array
 *     scalar or an instance of (a sub-class of) ndarray with a dimensionality
 *     of zero.
 * \internal Calls PyArray_CheckScalar.
 */
bool
is_array_scalar(python::object const & obj);

//______________________________________________________________________________
/**
 * \brief Checks of the given boost::python::object object is an instance of the
 *     ndarray class (or an instance of a derived subclass of ndarray).
 * \internal Calls PyArray_Check.
 */
bool
is_ndarray(python::object const & obj);

//______________________________________________________________________________
/**
 * \brief Copies the data from the source ndarray into the destination ndarray
 *     using the PyArray_CopyInto C-API function.
 *     It performs a data-type conversion if necessary. The shape of src must be
 *     broadcastable to the shape of dest. The data areas of dest and src must
 *     not overlap.
 *     It returns true, after success and false otherwise.
 */
bool
copy_into(ndarray & dst, ndarray const & src);

}// namespace numpy
}// namespace boost

// Make boost::python::extract<ndarray>(py_obj_ptr) work.
BOOST_NUMPY_OBJECT_MANAGER_TRAITS(boost::numpy::ndarray);

#endif // !BOOST_NUMPY_NDARRAY_HPP_INCLUDED
