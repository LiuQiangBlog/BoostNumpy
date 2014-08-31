/**
 * $Id$
 *
 * Copyright (C)
 * 2014 - $Date$
 *     Martin Wolf <boostnumpy@martin-wolf.org>
 *
 * \file    boost/numpy/dstream/mapping/converter/arg_type_to_core_shape.hpp
 * \version $Revision$
 * \date    $Date$
 * \author  Martin Wolf <boostnumpy@martin-wolf.org>
 *
 * \brief This file defines the arg_type_to_core_shape template for
 *        converting a function's argument type to a core shape type.
 *
 *        This file is distributed under the Boost Software License,
 *        Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *        http://www.boost.org/LICENSE_1_0.txt).
 */
#ifndef BOOST_NUMPY_DSTREAM_MAPPING_CONVERTER_ARG_TYPE_TO_CORE_SHAPE_HPP_INCLUDED
#define BOOST_NUMPY_DSTREAM_MAPPING_CONVERTER_ARG_TYPE_TO_CORE_SHAPE_HPP_INCLUDED

#include <boost/mpl/assert.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_scalar.hpp>

#include <boost/python/tuple.hpp>

#include <boost/numpy/mpl/is_std_vector_of_scalar.hpp>
#include <boost/numpy/dstream/dim.hpp>

#include <boost/numpy/dstream/mapping/detail/core_shape.hpp>

// Include some provided converters.
#include <boost/numpy/dstream/mapping/converter/arg_type_to_core_shape/std_vector_of_bp_object.hpp>

namespace boost {
namespace numpy {
namespace dstream {
namespace mapping {
namespace converter {

template <class T, class Enable=void>
struct arg_type_to_core_shape
{
    // The arg_type_to_core_shape needs to be specialized.
    // Trigger a compilation error with a meaningful message.
    BOOST_MPL_ASSERT_MSG(false,
        THE_arg_type_to_core_shape_CONVERTER_NEED_TO_BE_SPECIALIZED_FOR_FUNCTION_ARGUMENT_TYPE_T, (T));
};

namespace detail {

template <class T, class Enable=void>
struct scalar_to_core_shape
{
    typedef mapping::detail::core_shape<0>::shape<>
            type;
};

template <class T, class Enable=void>
struct std_vector_of_scalar_to_core_shape
{
    typedef mapping::detail::core_shape<1>::shape< dim::I >
            type;
};

template <class T>
struct select_arg_type_to_core_shape
{
    typedef typename remove_reference<T>::type
            bare_t;

    // Note: We need to use the if_ template here. If we would use the eval_if
    //       template, always one of the two if blocks (of each if !!) gets
    //       evaluated. But the evaluation must happen AFTER the converter was
    //       selected.
    typedef typename boost::mpl::if_<
              typename is_scalar<bare_t>::type
            , scalar_to_core_shape<T>

            , typename boost::mpl::if_<
                typename numpy::mpl::is_std_vector_of_scalar<bare_t>::type
              , std_vector_of_scalar_to_core_shape<T>

              , ::boost::numpy::dstream::mapping::converter::arg_type_to_core_shape<T>
              >::type
            >::type
            apply;
};

template <class T>
struct arg_type_to_core_shape
{
    typedef typename select_arg_type_to_core_shape<T>::apply::type
            type;
};

}// namespace detail

}// namespace converter
}// namespace mapping
}// namespace dstream
}// namespace numpy
}// namespace boost

#endif // !BOOST_NUMPY_DSTREAM_MAPPING_CONVERTER_ARG_TYPE_TO_CORE_SHAPE_HPP_INCLUDED
