cmake_minimum_required(VERSION 3.7)

set(QGAMAPLUGIN adjustmentdemoplugin)

project(${QGAMAPLUGIN} LANGUAGES CXX)

message("-- Linking ${QGAMAPLUGIN}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Gui Sql Widgets REQUIRED)

set(QGAMAPATH   ../../qgama)
set(GAMALIBPATH ../../gama/lib)
include_directories(${QGAMAPATH} ${GAMALIBPATH})

add_library(${QGAMAPLUGIN} SHARED
  ${QGAMAPATH}/qgamainterfaces.h
  ${QGAMAPLUGIN}.cpp
  ${QGAMAPLUGIN}.h

  # ${GAMALIBPATH}/gnu_gama/local/angobs.h
  # ${GAMALIBPATH}/gnu_gama/local/bearing.cpp
  # ${GAMALIBPATH}/gnu_gama/local/bearing.h
  # ${GAMALIBPATH}/gnu_gama/local/html.cpp
  # ${GAMALIBPATH}/gnu_gama/local/html.h
  # ${GAMALIBPATH}/gnu_gama/local/cluster.h
  # ${GAMALIBPATH}/gnu_gama/local/exception.h
  # ${GAMALIBPATH}/gnu_gama/local/float.h
  # ${GAMALIBPATH}/gnu_gama/local/itstream.h
  ${GAMALIBPATH}/gnu_gama/local/language.cpp
  ${GAMALIBPATH}/gnu_gama/local/language.h
  # ${GAMALIBPATH}/gnu_gama/local/svg.cpp
  # ${GAMALIBPATH}/gnu_gama/local/svg.h
  # ${GAMALIBPATH}/gnu_gama/local/acord/approx_heights.cpp
  # ${GAMALIBPATH}/gnu_gama/local/acord/approx_heights.h
  # ${GAMALIBPATH}/gnu_gama/local/acord/approx_vectors.cpp
  # ${GAMALIBPATH}/gnu_gama/local/acord/approx_vectors.h
  # ${GAMALIBPATH}/gnu_gama/local/acord/acord2.cpp
  # ${GAMALIBPATH}/gnu_gama/local/acord/acord2.h
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordalgorithm.cpp
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordalgorithm.h
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordazimuth.cpp
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordazimuth.h
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordhdiff.cpp
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordhdiff.h
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordintersection.cpp
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordintersection.h
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordpolar.cpp
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordpolar.h
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordstatistics.h
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordstatistics.cpp
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordtraverse.cpp
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordtraverse.h
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordvector.cpp
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordvector.h
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordweakchecks.cpp
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordweakchecks.h
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordzderived.cpp
  # ${GAMALIBPATH}/gnu_gama/local/acord/acordzderived.h
  # ${GAMALIBPATH}/gnu_gama/local/acord/reduce_observations.cpp
  # ${GAMALIBPATH}/gnu_gama/local/acord/reduce_observations.h
  # ${GAMALIBPATH}/gnu_gama/local/acord/reduce_to_ellipsoid.cpp
  # ${GAMALIBPATH}/gnu_gama/local/acord/reduce_to_ellipsoid.h
  # ${GAMALIBPATH}/gnu_gama/local/format.cpp
  # ${GAMALIBPATH}/gnu_gama/local/format.h
  # ${GAMALIBPATH}/gnu_gama/local/gamadata.cpp
  # ${GAMALIBPATH}/gnu_gama/local/gamadata.h
  # ${GAMALIBPATH}/gnu_gama/local/lcoords.cpp
  # ${GAMALIBPATH}/gnu_gama/local/lcoords.h
  # ${GAMALIBPATH}/gnu_gama/local/local_linearization.cpp
  # ${GAMALIBPATH}/gnu_gama/local/local_linearization.h
  # ${GAMALIBPATH}/gnu_gama/local/localnetwork2sql.cpp
  # ${GAMALIBPATH}/gnu_gama/local/localnetwork2sql.h
  # ${GAMALIBPATH}/gnu_gama/local/lpoint.h
  # ${GAMALIBPATH}/gnu_gama/local/median/g2d_cogo.cpp
  # ${GAMALIBPATH}/gnu_gama/local/median/g2d_cogo.h
  # ${GAMALIBPATH}/gnu_gama/local/median/g2d_coordinates.cpp
  # ${GAMALIBPATH}/gnu_gama/local/median/g2d_coordinates.h
  # ${GAMALIBPATH}/gnu_gama/local/median/g2d_exception.h
  # ${GAMALIBPATH}/gnu_gama/local/median/g2d_helper.cpp
  # ${GAMALIBPATH}/gnu_gama/local/median/g2d_helper.h
  # ${GAMALIBPATH}/gnu_gama/local/median/g2d_point.cpp
  # ${GAMALIBPATH}/gnu_gama/local/median/g2d_point.h
  ${GAMALIBPATH}/gnu_gama/local/network.cpp
  ${GAMALIBPATH}/gnu_gama/local/network.h
  # ${GAMALIBPATH}/gnu_gama/local/orientation.cpp
  # ${GAMALIBPATH}/gnu_gama/local/orientation.h
  # ${GAMALIBPATH}/gnu_gama/local/results/text/adjusted_observations.h
  # ${GAMALIBPATH}/gnu_gama/local/results/text/adjusted_unknowns.h
  # ${GAMALIBPATH}/gnu_gama/local/results/text/approximate_coordinates.h
  # ${GAMALIBPATH}/gnu_gama/local/results/text/error_ellipses.h
  # ${GAMALIBPATH}/gnu_gama/local/results/text/fixed_points.h
  # ${GAMALIBPATH}/gnu_gama/local/results/text/general_parameters.h
  # ${GAMALIBPATH}/gnu_gama/local/results/text/network_description.h
  # ${GAMALIBPATH}/gnu_gama/local/results/text/outlying_abs_terms.h
  # ${GAMALIBPATH}/gnu_gama/local/results/text/reduced_observations.h
  # ${GAMALIBPATH}/gnu_gama/local/results/text/reduced_observations_to_ellipsoid.h
  # ${GAMALIBPATH}/gnu_gama/local/results/text/residuals_observations.h
  # ${GAMALIBPATH}/gnu_gama/local/results/text/underline.cpp
  # ${GAMALIBPATH}/gnu_gama/local/results/text/underline.h
  # ${GAMALIBPATH}/gnu_gama/local/test_linearization_visitor.cpp
  # ${GAMALIBPATH}/gnu_gama/local/test_linearization_visitor.h
  # ${GAMALIBPATH}/gnu_gama/local/local_revision.cpp
  # ${GAMALIBPATH}/gnu_gama/local/local_revision.h
  # ${GAMALIBPATH}/gnu_gama/local/matvec.h
  # ${GAMALIBPATH}/gnu_gama/local/medianf.h
  ${GAMALIBPATH}/gnu_gama/local/observation.cpp
  ${GAMALIBPATH}/gnu_gama/local/observation.h
  # ${GAMALIBPATH}/gnu_gama/local/pointid.cpp
  # ${GAMALIBPATH}/gnu_gama/local/pointid.h
  # ${GAMALIBPATH}/gnu_gama/local/readsabw.h
  # ${GAMALIBPATH}/gnu_gama/local/skipcomm.cpp
  # ${GAMALIBPATH}/gnu_gama/local/skipcomm.h
  # ${GAMALIBPATH}/gnu_gama/local/sqlitereader.h
  # ${GAMALIBPATH}/gnu_gama/local/sqlitereader.cpp
  # ${GAMALIBPATH}/gnu_gama/local/writevisitor.h
  # ${GAMALIBPATH}/gnu_gama/local/xmlerror.h
  # ${GAMALIBPATH}/gnu_gama/local/xmlerror.cpp
)

target_link_libraries(${QGAMAPLUGIN} PRIVATE
  Qt6::Core Qt6::Gui Qt6::Sql Qt6::Widgets
)

target_compile_definitions(${QGAMAPLUGIN} PRIVATE ${QGAMAPLUGIN}_LIBRARY)

set_target_properties(${QGAMAPLUGIN} PROPERTIES LINKER_LANGUAGE CXX)
