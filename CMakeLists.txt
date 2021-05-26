cmake_minimum_required(VERSION 3.17)
project(cub_array)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/third-party"
)

add_executable(cub_array
        include/cub/array/ObjectArray.h
        include/cub/base/Placement.h
        test/ut/PlacementTest.cpp
        test/ut/catch.cpp include/cub/base/DeduceSizeType.h test/ut/DeduceSizeTypeTest.cpp include/cub/array/detail/ReadOnlyArrayLike.h test/ut/ReadOnlyArrayLikeTest.cpp include/cub/base/SizeOfArray.h include/cub/array/detail/ArrayLike.h include/cub/array/detail/ArrayHolder.h include/cub/array/detail/ObjectTrait.h test/ut/ObjectArrayTest.cpp include/cub/array/ArrayView.h test/ut/ArrayViewTest.cpp include/cub/array/ArraySortObject.h test/ut/ArraySortObjectTest.cpp include/cub/array/ArrayAgingUpdater.h test/ut/ArrayAgingUpdaterTest.cpp include/cub/array/detail/LessChecker.h include/cub/array/ValueArray.h include/cub/array/OrderedArrayIndex.h include/cub/array/detail/GenericOrderedArray.h include/cub/array/OrderedArray.h test/ut/OrderedArrayTest.cpp include/cub/array/OrderedObjectArray.h)