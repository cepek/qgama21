/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2009, 2010, 2012  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ library.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  $
*/

#include "xml2txt.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <gnu_gama/xml/localnetwork_adjustment_results.h>
#include <gnu_gama/version.h>
#include <gnu_gama/local/language.h>
#include <gnu_gama/local/results/text/underline.h>
#include <gnu_gama/statan.h>
#include <gnu_gama/gon2deg.h>
#include <gnu_gama/outstream.h>
#include <gnu_gama/size_to.h>

using namespace GNU_gama::local;
typedef GNU_gama::LocalNetworkAdjustmentResults Adjustment;

void GNU_gama::xml2txt_general_parameters(std::ostream& cout, const Adjustment& adj)
{
  using std::setw;
  using std::ios_base;
  using std::setprecision;

  cout << T_GaMa_Adjustment_of_geodetic_network << "        "
       << T_GaMa_version
       << adj.network_general_parameters.gama_local_version     << "-"
       << adj.network_general_parameters.gama_local_algorithm   << " / "
       << adj.network_general_parameters.gama_local_compiler    << "\n"
       << underline(T_GaMa_Adjustment_of_geodetic_network, '*') << "\n"
       << "http://www.gnu.org/software/gama/\n\n\n";


  if (!adj.description.empty())
    {
      cout << T_GaMa_network_description << '\n'
           << underline(T_GaMa_network_description, '*') << '\n'
           << adj.description << "\n\n";

      if ((*adj.description.rbegin()) != '\n')
        cout << '\n';
    }

  cout << T_GaMa_General_solution_parameters << "\n"
       << underline(T_GaMa_General_solution_parameters, '*') << "\n\n";
  int w0 = 0, w_ = 8;
  {
    int n;
    n = size_to<int>(strlen(T_GaMa_gpar1_coordinates));              if (n > w0) w0 = n;
    n = size_to<int>(strlen(T_GaMa_gpar1_adjusted_coordinates));     if (n > w0) w0 = n;
    n = size_to<int>(strlen(T_GaMa_gpar1_constrained_coordinates));  if (n > w0) w0 = n;
    n = size_to<int>(strlen(T_GaMa_gpar1_fixed_coordinates));        if (n > w0) w0 = n;
    n = size_to<int>(strlen(T_GaMa_gpar1_total));                    if (n > w0) w0 = n;
  }

  cout.setf(ios_base::left,  ios_base::adjustfield);
  cout << setw(w0) << T_GaMa_gpar1_coordinates << " ";
  cout.setf(ios_base::right, ios_base::adjustfield);
  cout << setw(w_+1) << "xyz"
       << setw(w_-1) << "xy"
       << setw(w_)   << "z"  << "\n\n";

  const int a_xyz = adj.coordinates_summary.adjusted.xyz;
  const int a_xy  = adj.coordinates_summary.adjusted.xy;
  const int a_z   = adj.coordinates_summary.adjusted.z;
  const int c_xyz = adj.coordinates_summary.constrained.xyz;
  const int c_xy  = adj.coordinates_summary.constrained.xy;
  const int c_z   = adj.coordinates_summary.constrained.z;
  const int f_xyz = adj.coordinates_summary.fixed.xyz;
  const int f_xy  = adj.coordinates_summary.fixed.xy;
  const int f_z   = adj.coordinates_summary.fixed.z;

  cout.setf(ios_base::left,  ios_base::adjustfield);
  cout << setw(w0) << T_GaMa_gpar1_adjusted_coordinates << ":";
  cout.setf(ios_base::right, ios_base::adjustfield);
  cout << setw(w_)  << a_xyz
       << setw(w_)  << a_xy
       << setw(w_)  << a_z
       << '\n';
  cout.setf(ios_base::left,  ios_base::adjustfield);
  cout << setw(w0) << T_GaMa_gpar1_constrained_coordinates << ":";
  cout.setf(ios_base::right, ios_base::adjustfield);
  cout << setw(w_)  << c_xyz
       << setw(w_)  << c_xy
       << setw(w_)  << c_z
       << '\n';
  cout.setf(ios_base::left,  ios_base::adjustfield);
  cout << setw(w0) << T_GaMa_gpar1_fixed_coordinates << ":";
  cout.setf(ios_base::right, ios_base::adjustfield);
  cout << setw(w_)  << f_xyz
       << setw(w_)  << f_xy
       << setw(w_)  << f_z
       << '\n';

  for (int ii=0; ii<w0+1+3*w_+1; ii++) cout << '-';
  cout << "\n";

  cout.setf(ios_base::left,  ios_base::adjustfield);
  cout << setw(w0) << T_GaMa_gpar1_total << ":";
  cout.setf(ios_base::right, ios_base::adjustfield);
  cout << setw(w_)  << (a_xyz + f_xyz)
       << setw(w_)  << (a_xy  + f_xy )
       << setw(w_)  << (a_z   + f_z  )
       << "\n\n";

  int w1 = 0;
  {
    int n;
    // n = strlen(T_GaMa_gpar1_computed_points);  if (n > w1) w1 = n;
    // n = strlen(T_GaMa_gpar1_fixed_points);     if (n > w1) w1 = n;
    // n = strlen(T_GaMa_gpar1_computed_heights); if (n > w1) w1 = n;
    // n = strlen(T_GaMa_gpar1_fixed_heights);    if (n > w1) w1 = n;
    // n = strlen(T_GaMa_gpar1_points_total);     if (n > w1) w1 = n;
    n = size_to<int>(strlen(T_GaMa_gpar1_directions));       if (n > w1) w1 = n;
    n = size_to<int>(strlen(T_GaMa_gpar1_angles));           if (n > w1) w1 = n;
    n = size_to<int>(strlen(T_GaMa_gpar1_distances));        if (n > w1) w1 = n;
    n = size_to<int>(strlen(T_GaMa_gpar1_observed_coords));  if (n > w1) w1 = n;
    n = size_to<int>(strlen(T_GaMa_gpar1_leveling_diffs));   if (n > w1) w1 = n;
    n = size_to<int>(strlen(T_GaMa_gpar1_z_angles));         if (n > w1) w1 = n;
    n = size_to<int>(strlen(T_GaMa_gpar1_s_dists));          if (n > w1) w1 = n;
    n = size_to<int>(strlen(T_GaMa_gpar1_obs_total));        if (n > w1) w1 = n;
    n = size_to<int>(strlen(T_GaMa_gpar1_equations));        if (n > w1) w1 = n;
    n = size_to<int>(strlen(T_GaMa_gpar1_redundancy));       if (n > w1) w1 = n;
  }
  int w2 = 0;
  {
    int n;
    // n = strlen(T_GaMa_gpar2_constrained_points);  if (n > w2) w2 = n;
    // n = strlen(T_GaMa_gpar2_constrained_heights); if (n > w2) w2 = n;
    n = size_to<int>(strlen(T_GaMa_gpar2_bearings));            if (n > w2) w2 = n;
    n = size_to<int>(strlen(T_GaMa_gpar2_number_of_unknowns));  if (n > w2) w2 = n;
    n = size_to<int>(strlen(T_GaMa_gpar2_network_defect));      if (n > w2) w2 = n;
  }
  const char* tab_sep = "            ";

  const int pocdel  = adj.observations_summary.distances;
  const int pocsmer = adj.observations_summary.directions;
  const int pocuhl  = adj.observations_summary.angles;
  const int pocsour = adj.observations_summary.xyz_coords;
  const int pocnivp = adj.observations_summary.h_diffs;
  const int poczeni = adj.observations_summary.z_angles;
  const int pocsikm = adj.observations_summary.s_dists;
  // number of vectors is not printed in gama-local text output
  const int pocvekt = adj.observations_summary.vectors;

  const int observations = pocdel + pocsmer + pocuhl + pocsour
    + pocnivp + poczeni + pocsikm + pocvekt;

  const int pocosn  = size_to<int>(adj.orientations.size());

  if (pocosn)
    {
      cout << set_width(T_GaMa_gpar1_directions, w1) << ":"
           << setw(6) << pocsmer << tab_sep
           << set_width(T_GaMa_gpar2_bearings, w2) << ":"
           << setw(6) << pocosn << '\n';
    }
  if (pocuhl)
    {
      cout << set_width(T_GaMa_gpar1_angles, w1) << ":"
           << setw(6) << pocuhl << '\n';
    }
  if (pocdel)
    {
      cout << set_width(T_GaMa_gpar1_distances, w1) << ":"
           << setw(6) << pocdel << '\n';
    }
  if (pocsour)
    {
      cout << set_width(T_GaMa_gpar1_observed_coords, w1) << ":"
           << setw(6) << pocsour << '\n';
    }
  if (pocnivp && (pocnivp != observations))
    {
      cout << set_width(T_GaMa_gpar1_leveling_diffs, w1) << ":"
           << setw(6) << pocnivp << '\n';
    }
  if (poczeni)
    {
      cout << set_width(T_GaMa_gpar1_z_angles, w1) << ":"
           << setw(6) << poczeni << '\n';
    }
  if (pocsikm)
    {
      cout << set_width(T_GaMa_gpar1_s_dists, w1) << ":"
           << setw(6) << pocsikm << '\n';
    }
  int types = 0;
  if (pocsmer) types++;
  if (pocdel)  types++;
  if (pocuhl)  types++;
  if (pocsour) types++;
  if (pocnivp) types++;
  if (poczeni) types++;
  if (pocsikm) types++;
  if (types != 1)
    cout << set_width(T_GaMa_gpar1_obs_total, w1) << ":"
         << setw(6) << observations << "\n";
  cout << '\n';


  cout << set_width(T_GaMa_gpar1_equations, w1) << ":"
       << setw(6) << observations     << tab_sep
       << set_width(T_GaMa_gpar2_number_of_unknowns, w2) << ":"
       << setw(6) << adj.project_equations.unknowns
       << '\n'
       << set_width(T_GaMa_gpar1_redundancy, w1) << ":"
       << setw(6) << adj.project_equations.degrees_of_freedom << tab_sep
       << set_width(T_GaMa_gpar2_network_defect, w2) << ":"
       << setw(6) << adj.project_equations.defect
       << '\n';
  cout.setf(ios_base::fixed, ios_base::floatfield);


  cout << "\n"
       << T_GaMa_m0_apriori << ":"
       << setprecision(2) << setw(9) << adj.standard_deviation.apriori << '\n';
  cout << T_GaMa_m0_empirical << ":"
       << setprecision(2) << setw(9)
       << (adj.project_equations.degrees_of_freedom > 0 ?
           sqrt(adj.project_equations.sum_of_squares /
                adj.project_equations.degrees_of_freedom) : 0);
  cout.setf(ios_base::scientific, ios_base::floatfield);
  cout << "         "
       << "[pvv] : "
       << setprecision(5)<< adj.project_equations.sum_of_squares
       << '\n';


  cout.setf(ios_base::fixed, ios_base::floatfield);
  cout << "\n";
  cout << T_GaMa_During_statistical_analysis_we_work << "\n\n"
       << (adj.standard_deviation.using_aposteriori ?
           T_GaMa_statan_with_empirical_standard_deviation :
           T_GaMa_statan_with_apriori_standard_deviation);
  cout << setprecision(2)
       << (adj.standard_deviation.using_aposteriori ?
           adj.standard_deviation.aposteriori : adj.standard_deviation.apriori)
       << "\n"
       <<  T_GaMa_statan_with_confidence_level
       << setprecision(0) << adj.standard_deviation.probability*100
       << " %\n\n";

  if (const int nadb = adj.project_equations.degrees_of_freedom)
    {
      const double alfa_pul = (1 - adj.standard_deviation.probability)/2;
      if (adj.standard_deviation.using_aposteriori)
        {
          double testm0 = adj.standard_deviation.aposteriori
            / adj.standard_deviation.apriori;
          double lower = sqrt(GNU_gama::Chi_square(1-alfa_pul,nadb)/nadb);
          double upper = sqrt(GNU_gama::Chi_square(  alfa_pul,nadb)/nadb);

          cout << T_GaMa_Ratio_empirical_to_apriori << setprecision(3)
               << testm0 << '\n'
               << setprecision(0) << adj.standard_deviation.probability*100
               << " % " << T_GaMa_interval << " ("
               << setprecision(3) << lower
               << ", " << upper
               << ") "
               << (lower<testm0 && upper>testm0 ?
                   T_GaMa_interval_contains :
                   T_GaMa_interval_doesnt_contain)
               << "\n";


          double m0d=0, m0s=0, m0u=0;   // m0' from dists. / dirs. / angles
          double sqd=0, sqs=0, squ=0;   // sum of weight coefficients
          int    itd=0, its=0, itu=0;
          for (int i=0; i<observations; i++)
            {
              const Adjustment::Observation& obs = adj.obslist[i];
              double v = obs.residual();
              double q = obs.qrr;
              if (obs.xml_tag == "distance")
                {
                  itd = 1;
                  m0d += v*v;
                  sqd += q;
                }
              else if (obs.xml_tag == "direction")
                {
                  its = 1;
                  m0s += v*v;
                  sqs += q;
                }
              else if (obs.xml_tag == "angle")
                {
                  itu = 2;
                  m0u += v*v;
                  squ += q;
                }
            }
          if (itd) m0d = sqrt(m0d/sqd);
          if (its + itu) m0s = sqrt((m0s+m0u)/(sqs+squ));
          if (itd+its+itu > 1)
            {
              double ma = adj.standard_deviation.apriori;
              if (itd)
                cout << T_GaMa_m0_distances << m0d/ma << "   ";
              if (its+itu)
                {
                  switch (its+itu) {
                  case 1: cout << T_GaMa_m0_directions; break;
                  case 2: cout << T_GaMa_m0_angles; break;
                  case 3: cout << T_GaMa_m0_dirs_angs; break;
                  }
                  cout << m0s/ma;
                }
              cout << '\n';
            }
        }


      double stud_opr;
      double max_stud = 0;
      int imax = -1;
      for (int i=0; i<observations; i++)
        {
          const Adjustment::Observation& obs = adj.obslist[i];
          if (obs.qrr > 1e-4)
            {
              stud_opr = std::abs(obs.std_residual);
              if (stud_opr > max_stud)
                {
                  max_stud = stud_opr;
                  imax = i;
                }
            }
        }
      bool aprm0 = !adj.standard_deviation.using_aposteriori;
      double krit_opr;
      if (aprm0)
        krit_opr = GNU_gama::Normal(alfa_pul);
      else
        {
          double s = GNU_gama::Student(alfa_pul, nadb-1);
          double t = s*s;
          krit_opr = sqrt(nadb*t/(nadb-1+t));
        }

      if (nadb > 1 && imax > -1 && adj.standard_deviation.using_aposteriori)
        {
          const Adjustment::Observation& obs = adj.obslist[imax];
          double v = obs.residual();
          double q = obs.qrr;
          double m_0_red =
            sqrt(fabs(adj.project_equations.sum_of_squares-v*v/q)/(nadb-1));
          cout << "\n" << T_GaMa_Maximal_decrease_of_m0
              << setprecision(3) << m_0_red/adj.standard_deviation.apriori
              << "\n\n";
        }

      if (imax > -1)
        {
          cout.setf(ios_base::fixed, ios_base::floatfield);
          if (aprm0)
            cout << T_GaMa_Maximal_normalized_residual;
          else
            cout << T_GaMa_genpar_Maximal_studentized_residual;
          cout << setprecision(2) << max_stud;
          if (max_stud > krit_opr)
            cout << T_GaMa_genpar_exceeds;
          else
            cout << T_GaMa_genpar_doesnt_exceed;
          cout << T_GaMa_genpar_critical_value <<  krit_opr << "\n"
               << T_GaMa_genpar_on_significance_level
               << setprecision(0)
               << (1 - adj.standard_deviation.probability)*100
               << T_GaMa_genpar_for_observation_ind
               << imax+1 << "\n";

          const Adjustment::Observation& obs = adj.obslist[imax];
          cout << "<" <<  obs.xml_tag;
          if (obs.xml_tag == "angle")
            cout << " from\"" <<  obs.from  << "\""
                 << " left\"" <<  obs.left  << "\""
                 << " right\"" << obs.right << "\"";
          else
            cout << " from=\"" << obs.from << "\""
                 << " to=\""   << obs.to   << "\"";

          if (obs.xml_tag == "direction" || obs.xml_tag == "angle" ||
              obs.xml_tag == "z-angle")
            cout.precision(4);
          else
            cout.precision(3);

          cout << " val=\"" << obs.obs
               << "\"";
          // cout << " stdev=\"" << obs.stdev  !!! currently not available !!!
          //      << "\"";
          cout << " />\n";
        }
    }

  cout << "\n\n";
}




void GNU_gama::xml2txt_adjusted_parameters(std::ostream& cout,const Adjustment& adj)
{
  using std::setw;
  using std::ios_base;
  using std::setprecision;

  const int MAXWID  = 12;     // IS->maxw_id();
  const int MAXWUNK =  3;     // IS->maxw_unk();
  //const int MAXWOBS =  4;     // IS->maxw_obs();

  std::string prev_id;

  { /* fixed points */

    int pocpevb=0, pocpevv=0;
    {   // for ...
      auto N=adj.fixed_points.size();
      for (decltype(N) i=0 ; i<N; ++i)
        {
          const Adjustment::Point& point = adj.fixed_points[i];

          if (point.hxy) pocpevb++;
          if (point.hz ) pocpevv++;
        }
    }   // for ...

    if (pocpevb != 0 || pocpevv != 0)
      {
        cout << T_GaMa_Review_of_fixed_points << "\n"
             << underline(T_GaMa_Review_of_fixed_points, '*') << "\n\n";

        std::ostringstream ostr;
        ostr.width(MAXWID);
        ostr << T_GaMa_point;
        //- value stored to 'table' is never read
        //- int table=0;
        if (pocpevb)
          {
            ostr.width(13);
            ostr << "x   ";
            ostr.width(13+2);
            ostr << "y   ";
            //- table += 2*13 + 2;
          }
        if (pocpevv)
          {
            if (pocpevb)
              {
                ostr << "  ";
                //- table += 2;
              }
            ostr.width(13);
            ostr << "z   ";
            //- table += 13;
          }
        cout << ostr.str() << '\n';
        auto oss = ostr.str().size();
        for (decltype(oss) i=0; i<oss; i++) cout << '=';
        cout << "\n\n";

        for (int i=0, N=adj.fixed_points.size(); i<N; ++i)
        {
          const Adjustment::Point& point = adj.fixed_points[i];
          cout.width(MAXWID); //IS->maxw_id());

          cout << point.id;
          if (point.hxy)
            {
              cout.precision(3);
              cout.width(13);
              cout << point.x;
              cout << "  ";
              cout.width(13);
              cout << point.y;
            }
          if (point.hz)
            {
              if (pocpevb && !point.hxy)
                {
                  cout.width(2*13+2);
                  cout << " ";
                }
              cout.precision(3);
              if (pocpevb)  cout << "  ";
              cout.width(13);
              cout << point.z;
          }

          cout << "\n";
        }
      }
    cout << "\n\n";
  }

  double kki         = adj.standard_deviation.confidence_scale;
  bool   coordinates = false;
  bool   heights     = false;
  auto aps1 = adj.adjusted_points.size();
  for (decltype (aps1) i=0; i<aps1; i++)
    {
      if (adj.adjusted_points[i].hxy) coordinates = true;
      if (adj.adjusted_points[i].hz ) heights     = true;
    }
  if (coordinates)
    { /* Adjusted unknowns */

      cout << T_GaMa_adjunk_Review_of_unknowns_coordidantes << "\n"
           << underline(T_GaMa_adjunk_Review_of_unknowns_coordidantes, '*')
           << "\n\n";
      cout.width(MAXWUNK);
      cout << "i" << " ";
      cout.width(MAXWID);
      cout << T_GaMa_point;
      cout << T_GaMa_adjunk_header1;
      for (int i=0; i<MAXWUNK+MAXWID+1; i++) cout << '=';
      cout << T_GaMa_adjunk_header2;
      cout.setf(ios_base::fixed, ios_base::floatfield);

      prev_id.clear();
      auto aps2 = adj.adjusted_points.size();
      for (decltype (aps2) ii=0; ii<aps2; ii++)
        {
          const Adjustment::Point point = adj.adjusted_points[ii];
          const Adjustment::Point aprox = adj.approximate_points[ii];

          if (point.hxy)
            {
              cout.width(MAXWUNK);
              cout << " " << " ";
              cout.width(MAXWID);
              if (prev_id != point.id)
                cout << point.id.c_str();
              else
                cout << " ";
              prev_id = point.id;
              double mx = std::sqrt(adj.cov(point.indx, point.indx));
              double my = std::sqrt(adj.cov(point.indy, point.indy));
              // double mp = sqrt(my*my+mx*mx);
              cout << "\n";

              cout.width(MAXWUNK);
              cout << adj.original_index[point.indx] << " ";
              cout.width(MAXWID);
              if (point.cxy)
                cout << "X" << " * ";
              else
                cout << "x" << "   ";
              cout.precision(5);
              cout.width(13);
              cout.precision(5);
              cout.width(13);
              cout << aprox.x << " ";
              cout.width(9);
              cout << (point.x - aprox.x) << " ";
              cout.width(13);
              cout << point.x << " ";
              cout.precision(1);
              cout.width(7);
              cout << mx << " ";
              cout.width(7);
              cout << mx*kki;
              cout << "\n";

              cout.width(MAXWUNK);
              cout << adj.original_index[point.indy] << " ";
              cout.width(MAXWID);
              if (point.cxy)
                cout << "Y" << " * ";
              else
                cout << "y" << "   ";
              cout.precision(5);
              cout.width(13);
              cout << aprox.y << " ";
              cout.width(9);
              cout << (point.y - aprox.y) << " ";
              cout.width(13);
              cout << point.y << " ";
              cout.precision(1);
              cout.width(7);
              cout << my << " ";
              cout.width(7);
              cout << my*kki;
              cout << "\n";
            }

          if (point.hz)
            {
              if (!point.hxy)
              {
                cout.width(MAXWUNK);
                cout << " " << " ";
                cout.width(MAXWID);
                if (prev_id != point.id)
                  cout << point.id.c_str();
                else
                  cout << " ";
                cout << '\n';
              }
              prev_id = point.id;

              cout.width(MAXWUNK);
              cout << adj.original_index[point.indz] << " ";
              cout.width(MAXWID);
              if (point.cz)
                cout << "Z" << " * ";
              else
                cout << "z" << "   ";

              cout.precision(5);
              cout.width(13);
              cout << aprox.z << " ";
              cout.width(9);
              cout << (point.z - aprox.z) << " ";
              cout.width(13);
              cout << point.z << " ";
              double mz = std::sqrt(adj.cov(point.indz, point.indz));
              cout.precision(1);
              cout.width(7);
              cout << mz << " ";
              cout.width(7);
              cout << mz*kki;
              cout << "\n";
            }

          cout << "\n";
        }

      if (adj.orientations.size())
        { /* Adjusted orientation shifts */

          double scale  = adj.gons ? 1.0 : 0.324;

          cout << "\n" << T_GaMa_adjunk_Review_of_unknowns_bearings << "\n"
               << underline(T_GaMa_adjunk_Review_of_unknowns_bearings, '*')
               << "\n\n";
          cout.width(MAXWUNK);
          cout << "i" << " ";
          cout.width(MAXWID);
          cout << T_GaMa_standpoint;
          if (!adj.gons) cout << "   ";
          cout << T_GaMa_adjunk_header3;
          for (int i=0; i<MAXWUNK+MAXWID+1; i++) cout << '=';
          if (adj.gons)
            cout  << T_GaMa_adjunk_header4;
          else
            cout <<
              "====== [d] ========= [d] ======== [d] =========== [ss] ===\n\n";

          auto aos = adj.orientations.size();
          for (decltype (aos) i=0; i<aos; i++)
            {
              const Adjustment::Orientation& orp = adj.orientations[i];

              cout.width(MAXWUNK);
              cout << adj.original_index[orp.index] << " " ;
              cout.width(MAXWID);
              const std::string& cb = orp.id;
              cout << cb.c_str() << "  ";
              double z = orp.approx;
              if (z <  0 ) z += 400;
              if (z > 400) z -= 400;
              cout.setf(ios_base::fixed, ios_base::floatfield);
              cout.precision(6);
              cout.width(12);
              if (adj.gons)
                cout << z << " ";
              else
                cout << GNU_gama::gon2deg(z, 0, 2) << " ";
              cout.width(10);
              double cor = orp.adj - orp.approx;
              if (adj.gons)
                cout << cor << " ";
              else
                cout << GNU_gama::gon2deg(cor, 2, 2) << " ";
              z = orp.adj;
              if (z <  0 ) z += 400;
              if (z > 400) z -= 400;
              cout.width(11);
              if (adj.gons)
                cout << z << " ";
              else
                cout << GNU_gama::gon2deg(z, 0, 2) << " ";
              cout.precision(1);
              cout.width(8);
              double mz = std::sqrt(adj.cov(orp.index, orp.index))*scale;
              cout << mz << " ";
              cout.width(7);
              cout << mz*kki;
              cout << '\n';
            }

          cout << '\n' << '\n';
        }

      { /* Mean errors and parameters of error ellipses */
        double elp_k = 0;
        {
          double alfa = (1 - adj.standard_deviation.probability);
          if (!adj.standard_deviation.using_aposteriori)
            {
              elp_k = std::sqrt(GNU_gama::Chi_square(float(alfa), 2));
            }
          else
            {
              int n = adj.project_equations.degrees_of_freedom;
              if (n > 0)
                elp_k = std::sqrt( n*(std::pow(alfa, -2.0/n) - 1));
              else
                elp_k = 0;
            }
        }

        std::string mp_max_id;
        double mp_max = -1, mp_mean = 0;
        int number_of_points = 0;

        bool has_coordinates = false;
        auto aps1 = adj.adjusted_points.size();
        for (decltype (aps1) i=0; i<aps1; i++)
          if (adj.adjusted_points[i].hxy)
            {
              has_coordinates = true;
              break;
            }

        if (has_coordinates)
          {
            cout.precision(1);

            cout
              << T_GaMa_errell_review_of_mean_errors_and_error_ellipses << "\n"
              << underline(T_GaMa_errell_review_of_mean_errors_and_error_ellipses,'*')
              << "\n\n";
            cout.width(MAXWID);
            cout << T_GaMa_point << ' ';
            cout << T_GaMa_errell_header1;
            for (int i=0; i<MAXWID+1; i++) cout << '=';
            if (adj.gons)
              cout << T_GaMa_errell_header2;
            else
              cout <<
                "== [mm] == [mm] ==== a [mm] b ==== [d] ===== a' [mm] b' ========";
            cout << "\n\n";

            auto aps2 = adj.adjusted_points.size();
            for (decltype(aps2) i=0; i<aps2; i++)
              {
                const Adjustment::Point& point = adj.adjusted_points[i];
                if (!point.hxy) continue;

                cout.width(MAXWID);
                cout << point.id.c_str() << ' ';

                double my = std::sqrt(adj.cov(point.indy, point.indy));
                double mx = std::sqrt(adj.cov(point.indx, point.indx));

                double mp = sqrt(my*my+mx*mx);
                if (mp < 1000)
                  cout.setf(ios_base::fixed, ios_base::floatfield);
                else
                  cout.setf(ios_base::scientific, ios_base::floatfield);
                cout.width(7);
                cout << mp << ' ';

                mp_mean += mp;
                if (mp > mp_max) {
                  mp_max = mp;
                  mp_max_id = point.id;
                }
                number_of_points++;

                double myx = mp/sqrt(2.0);
                cout.width(7);
                if (myx < 1000)
                  cout.setf(ios_base::fixed, ios_base::floatfield);
                else
                  cout.setf(ios_base::scientific, ios_base::floatfield);
                cout << myx << ' ' ;

                double a, b, alfa;
                { // IS->std_error_ellipse(point_id, a, b, alfa);
                  int iy = point.indy;
                  int ix = point.indx;
                  double m = adj.standard_deviation.using_aposteriori ?
                    adj.standard_deviation.aposteriori :
                    adj.standard_deviation.apriori;

                  double m2 = m*m;
                  double cyy = adj.cov(iy,iy)/m2;
                  double cyx = adj.cov(iy,ix)/m2;
                  double cxx = adj.cov(ix,ix)/m2;
                  double c = std::sqrt((cxx-cyy)*(cxx-cyy) + 4*cyx*cyx);
                  b = (cyy+cxx-c)/2;
                  if (b < 0) b = 0;

                  a = m * std::sqrt(b+c);
                  b = m * std::sqrt(b);
                  if (fabs(c) < 1e-5) // c == 0 is unsafe
                    alfa = 0;
                  else {
                    alfa = std::atan2(2*cyx, cxx-cyy)/2;
                    if (alfa < 0) alfa += M_PI;
                  }
                }

                cout.width(7);
                if (a < 1000)
                  cout.setf(ios_base::fixed, ios_base::floatfield);
                else
                  cout.setf(ios_base::scientific, ios_base::floatfield);
                cout << a << ' ';
                cout.width(7);
                if (b < 1000)
                  cout.setf(ios_base::fixed, ios_base::floatfield);
                else
                  cout.setf(ios_base::scientific, ios_base::floatfield);
                cout << b << ' ';
                cout.width(7);
                cout.setf(ios_base::fixed, ios_base::floatfield);
                double ea = alfa/M_PI*200;
                if (!adj.gons) ea *= 360.0/400;
                cout << ea << ' ';

                if (mp < 1000 && mp > 1e-3)
                  {       // ********* testing noise (coordinates are OK)
                    double ak = a*elp_k;
                    double bk = b*elp_k;
                    cout.width(7);
                    if (ak < 1000)
                      cout.setf(ios_base::fixed, ios_base::floatfield);
                    else
                      cout.setf(ios_base::scientific, ios_base::floatfield);
                    cout << ak << ' ';
                    cout.width(7);
                    if (bk < 1000)
                      cout.setf(ios_base::fixed, ios_base::floatfield);
                    else
                      cout.setf(ios_base::scientific, ios_base::floatfield);
                    cout << bk << ' ';

                    double g  = 0;
                    double dx = point.x - adj.approximate_points[i].x;
                    double dy = point.y - adj.approximate_points[i].y;
                    dx *= 1000;   // meters ==> millimetres
                    dy *= 1000;   //
                    double p1 = (dx*std::cos(alfa) + dy*std::sin(alfa));
                    double p2 = (dy*std::cos(alfa) - dx*std::sin(alfa));
                    if (ak > 0 && bk > 0 && bk > ak*1e-4)
                      {   // ***** testing noise (bk is practically 0)
                        p1 /= ak;
                        p2 /= bk;
                        g = std::sqrt(p1*p1 + p2*p2);
                      }
                    if (g < 1000)
                      cout.setf(ios_base::fixed, ios_base::floatfield);
                    else
                      cout.setf(ios_base::scientific, ios_base::floatfield);
                    cout.width(7);
                    cout << g;
                 }

                cout << "\n";
              }

            /* Maximal/mean position error */
            if (number_of_points >= 5)
              {
                cout.precision(1);
                cout << "\n"
                     << T_GaMa_adjunk_mean_position_error_maximal << mp_max
                     << T_GaMa_adjunk_mean_position_error_on_point
                     << mp_max_id << '\n'
                     << T_GaMa_adjunk_mean_position_error_average
                     << mp_mean/number_of_points
                     << " mm\n\n";
              }
          }
      }
    }


  if (heights && !coordinates)
    {
      cout << T_GaMa_adjunk_Review_of_unknowns_heights << "\n"
           << underline(T_GaMa_adjunk_Review_of_unknowns_heights, '*')
           << "\n\n";
      cout.width(MAXWUNK);
      cout << "i" << " ";
      cout.width(MAXWID);
      cout << T_GaMa_point;
      cout << T_GaMa_adjunk_header5;
      for (int i=0; i<MAXWUNK+MAXWID+1; i++) cout << '=';
      cout << T_GaMa_adjunk_header6;
      cout.setf(ios_base::fixed, ios_base::floatfield);

      auto aps = adj.adjusted_points.size();
      for (decltype(aps) i=0; i<aps; i++)
        {
          const Adjustment::Point& point = adj.adjusted_points[i];
          if (!point.hz) continue;

          cout.width(MAXWUNK);
          cout << adj.original_index[point.indz] << " ";
          cout.width(MAXWID);
          cout << point.id.c_str();
          if (point.cz)
            cout << " * ";
          else
            cout << "   ";
          cout.precision(5);
          cout.width(13);
          cout << adj.approximate_points[i].z << " ";
          cout.width(9);
          cout << (point.z - adj.approximate_points[i].z) << " ";
          cout.width(13);
          cout << point.z << " ";
          int j = point.indz;
          double mv = std::sqrt(adj.cov(j,j));
          cout.precision(1);
          cout.width(7);
          cout << mv << " ";
          cout.width(7);
          cout << mv*kki;
          cout << '\n';
        }

      cout << "\n\n";
    }
}




void GNU_gama::xml2txt_adjusted_observations(std::ostream& cout,const Adjustment& adj)
{
  using std::setw;
  using std::ios_base;
  using std::setprecision;

  const int MAXWID  = 12;     // IS->maxw_id();
  //const int MAXWUNK =  3;     // IS->maxw_unk();
  const int MAXWOBS =  4;     // IS->maxw_obs();

  const double kki   = adj.standard_deviation.confidence_scale;
  const double scale = adj.gons ? 1.0 : 0.324;

  /* adjusted observations */

  cout << "\n" << T_GaMa_adjobs_Adjusted_observations << "\n"
       << underline(T_GaMa_adjobs_Adjusted_observations, '*') << "\n\n";

  int minval = 12;
  int maxval = minval;   // maximal value field width (coordinates!)
  auto aos = adj.obslist.size();
  for (decltype (aos) i=0; i<aos; i++)
    {
      const Adjustment::Observation& obs = adj.obslist[i];
      int z = 0;
      double d = obs.obs;
      if (d < 0)
        {
          z = 1;
          d = -d;
        }
      if (d < 1e5) continue;
      z += 6;   // ... decimal point plus 5 digits
      do {
        z++;
        d /= 10;
      } while (d >= 1);
      if (z > maxval) maxval = z;
    }

  cout.width(MAXWOBS);
  cout << "i" << " ";
  cout.width(MAXWID);
  cout << T_GaMa_standpoint << " ";
  cout.width(MAXWID);
  cout << T_GaMa_target << "       ";
  cout.width(maxval);
  cout << T_GaMa_adjobs_observed << " ";
  cout.width(maxval);
  cout << T_GaMa_adjobs_adjusted << T_GaMa_adjobs_header1;
  {
    int kk = 13 + maxval-minval;
    for (int i=0; i < (MAXWOBS+2*MAXWID+kk); i++) cout << "=";
  }
  cout << T_GaMa_adjobs_value;
  {
    for (int i=minval; i<maxval; i++) cout << "=";
  }
  if (adj.gons)
    cout << "==== [m|g] ====== [mm|cc] ==\n\n";
  else
    cout << "==== [m|d] ====== [mm|ss] ==\n\n";

  std::string predcs="";  // previous standpoint ID
  auto aos2 = adj.obslist.size();
  for (decltype(aos2) i=0; i<aos2; i++)
    {
      const Adjustment::Observation& obs = adj.obslist[i];

      cout.width(MAXWOBS);
      cout << i+1 << " ";
      std::string cs = obs.from;
      cout.width(MAXWID);
      if (cs != predcs)
         cout << cs.c_str();
      else
         cout << " ";
      cout << " ";
      std::string cc = obs.to;
      if (obs.xml_tag == "angle") cc = obs.left;
      cout.width(MAXWID);
      cout << cc.c_str();
      cout.setf(ios_base::fixed, ios_base::floatfield);

      if (obs.xml_tag == "distance")
        {
          cout << T_GaMa_distance;
          cout.precision(5);
          cout.width(maxval);
          cout << obs.obs << " ";
          cout.width(maxval);
          cout << obs.adj << " ";
        }
      else if (obs.xml_tag == "direction")
        {
          cout << T_GaMa_direction;
          cout.precision(6);
          cout.width(maxval);
          double m = obs.obs;
          if (adj.gons)
            cout << m << " ";
          else
            cout << GNU_gama::gon2deg(m, 0, 2) << " ";
          cout.width(maxval);
          m = obs.adj;
          if (m < 0) m += 400;
          if (m >= 400) m -= 400;
          if (adj.gons)
            cout << m << " ";
          else
            cout << GNU_gama::gon2deg(m, 0, 2) << " ";
        }
      else if (obs.xml_tag == "angle")
        {
          cout << '\n';
          cout.width(MAXWOBS + 2 + 2*(MAXWID));
          cout << obs.right.c_str();
          cout << T_GaMa_angle;
          cout.precision(6);
          cout.width(maxval);
          double m = obs.obs;
          if (adj.gons)
            cout << m << " ";
          else
            cout << GNU_gama::gon2deg(m, 0, 2) << " ";
          cout.width(maxval);
          m = obs.adj;
          if (m < 0) m += 400;
          if (m >= 400) m -= 400;
          if (adj.gons)
            cout << m << " ";
          else
            cout << GNU_gama::gon2deg(m, 0, 2) << " ";
        }
      else if (obs.xml_tag == "slope-distance")
        {
          cout << T_GaMa_s_distance;
          cout.precision(5);
          cout.width(maxval);
          cout << obs.obs << " ";
          cout.width(maxval);
          cout << obs.adj << " ";
        }
      else if (obs.xml_tag == "zenith-angle")
        {
          cout << T_GaMa_z_angle;
          cout.precision(6);
          cout.width(maxval);
          double m = obs.obs;
          if (adj.gons)
            cout << m << " ";
          else
            cout << GNU_gama::gon2deg(m, 0, 2) << " ";
          cout.width(maxval);
          m = obs.adj;
          if (adj.gons)
            cout << m << " ";
          else
            cout << GNU_gama::gon2deg(m, 0, 2) << " ";
        }
      else if (obs.xml_tag == "coordinate-x")
        {
          cout << T_GaMa_x;
          cout.precision(5);
          cout.width(maxval);
          cout << obs.obs << " ";
          cout.width(maxval);
          cout << obs.adj << " ";
        }
      else if (obs.xml_tag == "coordinate-y")
        {
          cout << T_GaMa_y;
          cout.precision(5);
          cout.width(maxval);
          cout << obs.obs << " ";
          cout.width(maxval);
          cout << obs.adj << " ";
        }
      else if (obs.xml_tag == "coordinate-z")
        {
          cout << T_GaMa_z;
          cout.precision(5);
          cout.width(maxval);
          cout << obs.obs << " ";
          cout.width(maxval);
          cout << obs.adj << " ";
        }
      else if (obs.xml_tag == "height-diff")
        {
          cout << T_GaMa_levell;
          cout.precision(5);
          cout.width(maxval);
          cout << obs.obs << " ";
          cout.width(maxval);
          cout << obs.adj << " ";
        }
      else if (obs.xml_tag == "dx")
        {
          cout << T_GaMa_xdiff;
          cout.precision(5);
          cout.width(maxval);
          cout << obs.obs << " ";
          cout.width(maxval);
          cout << obs.adj << " ";
        }
      else if (obs.xml_tag == "dy")
        {
          cout << T_GaMa_ydiff;
          cout.precision(5);
          cout.width(maxval);
          cout << obs.obs << " ";
          cout.width(maxval);
          cout << obs.adj << " ";
        }
      else if (obs.xml_tag == "dz")
        {
          cout << T_GaMa_zdiff;
          cout.precision(5);
          cout.width(maxval);
          cout << obs.obs << " ";
          cout.width(maxval);
          cout << obs.adj << " ";
        }
      else
        {
          cout << "\n######  review/adjusted_observations.h - "
               << "unknown observation type " << obs.xml_tag << "\n\n";
        }

      cout.precision(1);
      cout.width(7);
      double ml = obs.stdev;
      if (obs.xml_tag == "direction" || obs.xml_tag == "angle" ||
          obs.xml_tag == "z-angle")
        {
          ml *= scale;
        }
      cout << ml << " ";
      cout.width(7);
      cout << ml*kki;

      cout << "\n";
      predcs = cs;
    }


  /* Residuals and analysis of observations */

  // class StOpSort {
  //
  //   GNU_gama::local::LocalNetwork* IS;
  //
  // public:
  //
  //   StOpSort(GNU_gama::local::LocalNetwork* is) : IS(is) {}
  //   bool operator()(int a, int b)
  //     {
  //       using namespace std;
  //       GNU_gama::local::Double sa = fabs(IS->studentized_residual(a));
  //       GNU_gama::local::Double sb = fabs(IS->studentized_residual(b));
  //       return sa > sb;
  //     }
  // };

  std::vector<int> outliers;
    int imax = -1;         // index of maximal studentized residual
  {
    double maxno = 0;
    for (int i=1; i<size_to<int>(adj.obslist.size()); i++)
      {
        const Adjustment::Observation& obs = adj.obslist[i];

        if (obs.f < 0.1) continue;

        double no = std::abs(obs.std_residual);
        if (no > maxno) {
          maxno = no;
          imax = i;
        }
        if (no > kki) outliers.push_back(i);
      }
    // if (outliers.size() > 0)
    //   sort(outliers.begin(), outliers.end(), StOpSort(IS));
  }

  for (int pass=1; pass; pass--)
    {
      if (pass)
        cout << "\n\n"
             << T_GaMa_resobs_Review_of_residuals_analysis_obs << "\n"
             << underline(T_GaMa_resobs_Review_of_residuals_analysis_obs, '*')
             << "\n\n";
      else
        cout << "\n\n"
             << T_GaMa_resobs_Outlying_observations << "\n"
             << underline(T_GaMa_resobs_Outlying_observations, '*') << "\n\n";

      cout.width(MAXWOBS);
      cout << "i" << " ";
      cout.width(MAXWID);
      cout << T_GaMa_standpoint << " ";
      cout.width(MAXWID);
      cout << T_GaMa_target
           << T_GaMa_resobs_header1;
      for (int i=0; i < (MAXWOBS + 2*MAXWID + 10); i++)  cout << "=";
      if (adj.gons)
        cout << "======== [mm|cc] =========== [mm|cc] ===\n\n";
      else
        cout << "======== [mm|ss] =========== [mm|ss] ===\n\n";


      std::string previous_id = "";    // previous standpoint ID
      const int NN = pass ? adj.obslist.size() : outliers.size();
      for (int ii=0; ii<NN; ii++)
        {
          int i = pass ? ii : outliers[ii];
          const Adjustment::Observation& obs = adj.obslist[i];

          cout.width(MAXWOBS);
          cout << i+1 << " ";
          cout.width(MAXWID);
          if (obs.from != previous_id)
            cout << obs.from.c_str();
          else
            cout << " ";
          previous_id = obs.from;
          cout << " ";
          std::string cc = obs.to;
          if (obs.xml_tag == "angle")  cc = obs.left;
          cout.width(MAXWID);
          cout << cc.c_str();
          cout.setf(ios_base::fixed, ios_base::floatfield);

          if (obs.xml_tag == "distance")
            {
              cout << T_GaMa_distance;
            }
          else if (obs.xml_tag == "direction")
            {
              cout << T_GaMa_direction;
            }
          else if (obs.xml_tag == "angle")
            {
              cout << '\n';
              cout.width(MAXWOBS + 2 + 2*MAXWID);
              cout << obs.right.c_str();
              cout << T_GaMa_angle;
            }
          else if (obs.xml_tag == "slope-distance")
            {
              cout << T_GaMa_s_distance;
            }
          else if (obs.xml_tag == "zenith-angle")
            {
              cout << T_GaMa_z_angle;
            }
          else if (obs.xml_tag == "coordinate-x")
            {
              cout << T_GaMa_x;
            }
          else if (obs.xml_tag == "coordinate-y")
            {
              cout << T_GaMa_y;
            }
          else if (obs.xml_tag == "coordinate-z")
            {
              cout << T_GaMa_z;
            }
          else if (obs.xml_tag == "height-diff")
            {
              cout << T_GaMa_levell;
            }
          else if (obs.xml_tag == "dx")
            {
              cout << T_GaMa_xdiff;
            }
          else if (obs.xml_tag == "dy")
            {
              cout << T_GaMa_ydiff;
            }
          else if (obs.xml_tag == "dz")
            {
              cout << T_GaMa_zdiff;
            }
          else
            {
              cout << "\n######  review/adjusted_observations.h - "
                   << "unknown observation type " << obs.xml_tag << "\n\n";
            }


          double f  = obs.f;
          double sc = 1.0;
          if (obs.xml_tag == "direction" || obs.xml_tag == "angle" ||
              obs.xml_tag == "z-angle")
            {
              sc = adj.gons ? 1.0 : 0.324;
            }

          cout.precision(1);
          cout.width(5);
          cout << f;
          if (f < 0.1)    cout << T_GaMa_resobs_no_control;   // uncontrolled
          else if (f < 5) cout << T_GaMa_resobs_weak_control; // weak control
          else            cout << "  ";
          cout << ' ';

          cout.precision(3);
          cout.width(9);
          cout << obs.residual()*sc << ' ';
          cout.precision(1);
          cout.width(4);
          if (f >= 0.1)
            {
              double no = std::abs(obs.std_residual);
              cout << no;

              if (i == imax)
                {
                  if (no > kki)  cout << T_GaMa_resobs_mc_max_critical;
                  else           cout << T_GaMa_resobs_mc_max;
                }
              else if (no > kki) cout << T_GaMa_resobs_mc_critical;
              else               cout << "   ";


              if ((f >=5 || (f >= 0.1 && no > kki)) &&
                  !obs.err_obs.empty() &&
                  !obs.err_adj.empty()
                  )
                {
                  double erro = std::atof(obs.err_obs.c_str()) * sc;
                  cout.width(7);
                  cout << erro;
                  double erra = std::atof(obs.err_adj.c_str()) * sc;
                  cout.width(7);
                  cout << erra;
                }
            }

          cout << "\n";
        }
    }
}

