/**
 * $Id$
 *
 * Copyright (C)
 * 2014 - $Date$
 *     Martin Wolf <boostnumpy@martin-wolf.org>
 *
 * \file    boost/numpy/dstream/wiring/return_to_core_shape_data.hpp
 * \version $Revision$
 * \date    $Date$
 * \author  Martin Wolf <boostnumpy@martin-wolf.org>
 *
 * \brief This file defines the return_to_core_shape_data template that should
 *        put a function's return value into the output arrays defined by the
 *        out mapping type and its core shapes.
 *
 *        This file is distributed under the Boost Software License,
 *        Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *        http://www.boost.org/LICENSE_1_0.txt).
 */
#if !BOOST_PP_IS_ITERATING

#ifndef BOOST_NUMPY_DSTREAM_WIRING_RETURN_TO_CORE_SHAPE_DATA_HPP_INCLUDED
#define BOOST_NUMPY_DSTREAM_WIRING_RETURN_TO_CORE_SHAPE_DATA_HPP_INCLUDED

#include <stdint.h>

#include <iostream>
#include <vector>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/iterate.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>

#include <boost/assert.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_scalar.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include <boost/numpy/limits.hpp>
#include <boost/numpy/detail/iter.hpp>
#include <boost/numpy/detail/utils.hpp>
#include <boost/numpy/dstream/mapping/detail/definition.hpp>
#include <boost/numpy/dstream/wiring/detail/utilities.hpp>

namespace boost {
namespace numpy {
namespace dstream {
namespace wiring {
namespace converter {

template <class WiringModelAPI, class OutMapping, class RT, class Enable=void>
struct return_to_core_shape_data
{
    typedef return_to_core_shape_data<WiringModelAPI, OutMapping, RT, Enable>
            type;

    // The return_to_core_shape_data needs to be specialized.
    // Trigger a compilation error with a meaningful message.
    BOOST_MPL_ASSERT_MSG(false,
        THE_return_to_core_shape_data_CONVERTER_NEED_TO_BE_SPECIALIZED_FOR_WIRING_MODEL_API_AND_FUNCTION_RETURN_TYPE_RT_AND_OUT_MAPPING_TYPE_OutMapping, (WiringModelAPI, RT, OutMapping));
};

namespace detail {

template <class VectorT, unsigned axis>
bool
multidim_std_vector_has_aligned_axis(VectorT const & v);

template <class VectorT>
bool
multidim_std_vector_has_aligned_axis<0>(VectorT const & v)
{
    v.size()
}

template <class VectorT, unsigned nd>
std::vector<intptr_t>
get_multidim_std_vector_shape(VectorT const & v);

template <class VectorT>
std::vector<intptr_t>
get_multidim_std_vector_shape<VectorT, 1>(VectorT const & v)
{
    std::vector<intptr_t> shape(1);
    shape[0] = v.size();

    return shape;
}

template <class VectorT>
std::vector<intptr_t>
get_multidim_std_vector_shape<VectorT, 2>(VectorT const & v)
{
    std::vector<intptr_t> shape(2);
    shape[0] = v.size();
    shape[1] = v[0].size();


    return shape;
}

//------------------------------------------------------------------------------
// The scalar_return_to_core_shape_data_impl template is used to put the
// function's scalar result data into the one and only output array.
// This makes only sense for scalar out mappings, which is ensured already by
// the select_scalar_return_to_core_shape_data_impl metafunction.
template <class WiringModelAPI, class OutMapping, class RT>
struct scalar_return_to_core_shape_data_impl
{
    typedef scalar_return_to_core_shape_data_impl<WiringModelAPI, OutMapping, RT>
            type;

    typedef typename WiringModelAPI::template out_arr_value_type<0>::type
            out_arr_value_t;

    static
    bool
    apply(
        RT result
      , numpy::detail::iter & iter
      , std::vector< std::vector<intptr_t> > const & out_core_shapes
    )
    {
        BOOST_ASSERT((out_core_shapes.size()    == 1 &&
                      out_core_shapes[0].size() == 1 &&
                      out_core_shapes[0][0]     == 1));

        out_arr_value_t & out_arr_value = *reinterpret_cast<out_arr_value_t *>(iter.get_data(0));
        out_arr_value = out_arr_value_t(result);

        return true;
    }
};

template <class WiringModelAPI, class OutMapping, class RT>
struct select_scalar_return_to_core_shape_data_impl
{
    typedef mapping::detail::out_mapping<OutMapping>
            out_mapping_utils;

    // Check if the output arity is 1.
    typedef typename out_mapping_utils::template arity_is_equal_to<1>::type
            is_unary_out_mapping;

    // Check if the output array has a scalar core shape.
    typedef typename out_mapping_utils::template array<0>::is_scalar::type
            is_scalar_out_array;

    // Check if the output array has a scalar data holding type.
    typedef typename is_scalar<WiringModelAPI::template out_arr_value_type<0>::type>::type
            is_scalar_out_array_data_type;

    typedef typename boost::mpl::if_<
              typename boost::mpl::and_<
                is_unary_out_mapping
              , is_scalar_out_array
              , is_scalar_out_array_data_type
              >::type
            , scalar_return_to_core_shape_data_impl<WiringModelAPI, OutMapping, RT>

            , numpy::mpl::unspecified
            >::type
            type;
};

//------------------------------------------------------------------------------
// The std_vector_of_scalar_return_to_core_shape_data_impl template is used to
// put the function's result data from a n-dimensional std::vector of scalar
// type into out_arity output arrays.
template <class WiringModelAPI, class OutMapping, class RT, unsigned nd, unsigned out_arity>
struct std_vector_of_scalar_return_to_core_shape_data_impl;

// Define specializations for dimensions I to Z, i.e. 18 dimensions for
// out_arity = 1.
#define BOOST_PP_ITERATION_PARAMS_1                                            \
    (4, (1, 18, <boost/numpy/dstream/wiring/converter/return_to_core_shape_data.hpp>, 1))
#include BOOST_PP_ITERATE()

// Define specializations for dimensions I to Z, i.e. 18 dimensions for
// out_arity >= 2.
#define BOOST_PP_ITERATION_PARAMS_1                                            \
    (4, (2, BOOST_NUMPY_LIMIT_OUTPUT_ARITY, <boost/numpy/dstream/wiring/converter/return_to_core_shape_data.hpp>, 2))
#include BOOST_PP_ITERATE()

template <class WiringModelAPI, class OutMapping, class RT, unsigned nd, unsigned out_arity>
struct select_std_vector_of_scalar_return_to_core_shape_data_impl
{
    typedef mapping::detail::out_mapping<OutMapping>
            out_mapping_utils;

    // At this point we know that RT is a nd-dimensional std::vector of scalar
    // and that out_arity is greater than 1 (because out_arity=1 is specialized
    // below). Thus, we will distribute the first dimension of the
    // nd-dimensional vector over the different output arrays.

    // First, we need to check if all the output arrays have the same
    // dimensionality of nd-1, because we will distribute the first axis of
    // the nd-dimensional result vector to the out_arity output arrays.
    typedef typename out_mapping_utils::template all_arrays_have_dim<nd-1>::type
            all_arrays_have_correct_dim;

    // Second, we need to check if all the output arrays have a scalar data
    // holding type.
    typedef typename wiring::detail::utilities<WiringModelAPI>::template all_out_arr_value_types<boost::is_scalar>::type
            all_out_arr_value_types_are_scalars;

    typedef typename boost::mpl::if_<
              typename boost::mpl::and_<
                all_arrays_have_correct_dim
              , all_out_arr_value_types_are_scalars
              >::type
            , std_vector_of_scalar_return_to_core_shape_data_impl<WiringModelAPI, OutMapping, RT, nd, out_arity>

            , numpy::mpl::unspecified
            >::type
            type;
};

// Specialization for out_arity = 1.
template <class WiringModelAPI, class OutMapping, class RT, unsigned nd>
struct select_std_vector_of_scalar_return_to_core_shape_data_impl<WiringModelAPI, OutMapping, RT, nd, 1>
{
    typedef mapping::detail::out_mapping<OutMapping>
            out_mapping_utils;

    // Check if the one-and-only output array has a core dimensionality equal
    // to nd.
    typedef typename out_mapping_utils::template array<0>::template has_dim<nd>::type
            has_correct_dim;

    // Check if the one-and-only output array has a scalar data holding type.
    typedef typename is_scalar<typename WiringModelAPI::template out_arr_value_type<0>::type>::type
            has_scalar_array_data_holding_type;

    typedef typename boost::mpl::if_<
              typename boost::mpl::and_<
                has_correct_dim
              , has_scalar_array_data_holding_type
              >::type
            , std_vector_of_scalar_return_to_core_shape_data_impl<WiringModelAPI, OutMapping, RT, nd, 1>

            , numpy::mpl::unspecified
            >::type
            type;
};

template <class WiringModelAPI, class OutMapping, class RT, class NestedRT, unsigned ND>
struct std_vector_return_to_core_shape_data
{
    typedef typename remove_reference<NestedRT>::type
            vector_t;
    typedef typename vector_t::value_type
            vector_value_t;
    typedef typename remove_reference<vector_value_t>::type
            vector_bare_value_t;

    typedef typename boost::mpl::if_<
              typename is_scalar<vector_bare_value_t>::type
            , typename select_std_vector_of_scalar_return_to_core_shape_data_impl<WiringModelAPI, OutMapping, RT, OutMapping::arity, ND>::type

              // TODO: Add check for bp::object vector value type.
            // Check if the std::vector's value type is a std::vector again, and
            // if so, keep track of the number of dimensions.
            , typename boost::mpl::eval_if<
                typename numpy::mpl::is_std_vector<vector_value_t>::type
              , std_vector_return_to_core_shape_data<WiringModelAPI, OutMapping, RT, vector_value_t, ND+1>

              , numpy::mpl::unspecified
              >::type
            >::type
            type;
};

template <class WiringModelAPI, class OutMapping, class RT>
struct select_return_to_core_shape_data_converter
{
    typedef typename remove_reference<RT>::type
            bare_rt;

    typedef mapping::detail::out_mapping<OutMapping>
            out_mapping_utils;

    typedef typename boost::mpl::if_<
              typename is_scalar<bare_rt>::type
            , select_scalar_return_to_core_shape_data_impl<WiringModelAPI, OutMapping, RT>

            // TODO: Add bp::object types.
            , typename boost::mpl::eval_if<
                typename numpy::mpl::is_std_vector<bare_rt>::type
              , std_vector_return_to_core_shape_data<WiringModelAPI, OutMapping, RT, RT, 1>

              , numpy::mpl::unspecified
              >::type
            >::type
            type;
};

template <class WiringModelAPI, class OutMapping, class RT>
struct return_to_core_shape_data_converter
{
    typedef typename select_return_to_core_shape_data_converter<WiringModelAPI, OutMapping, RT>::type
            builtin_converter_selector;
    typedef typename boost::mpl::eval_if<
              is_same<typename builtin_converter_selector::type, numpy::mpl::unspecified>
            , ::boost::numpy::dstream::wiring::converter::return_to_core_shape_data<WiringModelAPI, OutMapping, RT>
            , builtin_converter_selector
            >::type
            type;
};

}// namespace detail
}// namespace converter
}// namespace wiring
}// namespace dstream
}// namespace numpy
}// namespace boost

#endif // ! BOOST_NUMPY_DSTREAM_WIRING_RETURN_TO_CORE_SHAPE_DATA_HPP_INCLUDED
#else

#if BOOST_PP_ITERATION_FLAGS() == 1

#define ND BOOST_PP_ITERATION()

template <class WiringModelAPI, class OutMapping, class RT>
struct std_vector_of_scalar_return_to_core_shape_data_impl<WiringModelAPI, OutMapping, RT, ND, 1>
{
    // This implementation is used to put the ND-dimensional result into the
    // one and only output array (i.e. nd=ND and out_arity=1).
    typedef std_vector_of_scalar_return_to_core_shape_data_impl<WiringModelAPI, OutMapping, RT, ND, 1>
            type;

    typedef typename remove_reference<RT>::type
            vector_t;

    typedef typename WiringModelAPI::template out_arr_value_type<0>::type
            out_arr_value_t;

    static
    bool
    apply(
        RT result
      , numpy::detail::iter & iter
      , std::vector< std::vector<intptr_t> > const & out_core_shapes
    )
    {
        BOOST_ASSERT((out_core_shapes.size() == 1 && out_core_shapes[0].size() == ND));

        std::vector<intptr_t> const out_op_value_strides = iter.get_operand(0).get_strides_vector();

        // Check if the shape of the function result matches the shape of the
        // output array.
        std::vector<intptr_t> const result_shape = get_multidim_std_vector_shape<vector_t, ND>(result);
        if(result_shape != out_core_shapes[0])
        {
            std::cerr << "The shape of the function's "<<ND<<"-dimensional "
                      << "result vector "
                      << numpy::detail::std_vector_to_string<intptr_t>(result_shape)
                      << " must be "
                      << numpy::detail::std_vector_to_string<intptr_t>(out_core_shapes[0]) << "!"
                      << std::endl;
            return false;
        }
        //FIXME for multi dim.
        for(intptr_t i=0; i<N; ++i)
        {
            out_arr_value_t & out_arr_value = *reinterpret_cast<out_arr_value_t *>(iter.get_data(0) + i*out_op_value_stride);
            out_arr_value = out_arr_value_t(result[i]);
        }
        return true;
    }
};

#undef ND

#else
#if BOOST_PP_ITERATION_FLAGS() == 2

#define OUT_ARITY BOOST_PP_ITERATION()

template <class WiringModelAPI, class OutMapping, class RT>
struct std_vector_of_scalar_return_to_core_shape_data_impl<OutMapping, RT, 1, OUT_ARITY>
{
    // This implementation is used to put the 1-dimensional result into the
    // OUT_ARITY scalar output arrays.

    typedef std_vector_of_scalar_return_to_core_shape_data_impl<WiringModelAPI, OutMapping, RT, 1, OUT_ARITY>
            type;

    static
    bool
    apply(
        RT result
      , numpy::detail::iter & iter
      , std::vector< std::vector<intptr_t> > const & out_core_shapes
    )
    {
        // Check if the number of scalar values match the output arity.
        if(result.size() != OUT_ARITY)
        {
            std::cerr << "The size of the return vector "
                      << "("<< result.size() <<") does not match the output "
                      << "arity ("<< OUT_ARITY <<")!" << std::endl;
            return false;
        }

        #define BOOST_NUMPY_DSTREAM_DEF(z, n, data)                                     \
            typedef typename WiringModelAPI::template out_arr_value_type<n>::type       \
                    BOOST_PP_CAT(out_arr_value_t,n);                                    \
            BOOST_PP_CAT(out_arr_value_t,n) & BOOST_PP_CAT(out_arr_value,n) =           \
                *reinterpret_cast<BOOST_PP_CAT(out_arr_value_t,n) *>(iter.get_data(n)); \
            BOOST_PP_CAT(out_arr_value,n) = BOOST_PP_CAT(out_arr_value_t,n)(result[n]);
        BOOST_PP_REPEAT(OUT_ARITY, BOOST_NUMPY_DSTREAM_DEF, ~)
        #undef BOOST_NUMPY_DSTREAM_DEF
        return true;
    }
};

#undef OUT_ARITY

#endif // BOOST_PP_ITERATION_FLAGS() == 2
#endif // BOOST_PP_ITERATION_FLAGS() == 1

#endif // BOOST_PP_IS_ITERATING
