/*  Qt based GUI for GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2014, 2016, 2020  Ales Cepek <cepek@gnu.org>

    This file is a part of GNU Gama.

    GNU Gama is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GNU Gama is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "adjustment.h"
#include "constants.h"

#include <gnu_gama/local/observation.h>
#include <gnu_gama/local/network.h>
#include <gnu_gama/local/acord/acord2.h>
#include <gnu_gama/local/orientation.h>
#include <gnu_gama/local/gamadata.h>
#include <gnu_gama/xml/localnetworkxml.h>
#include <gnu_gama/local/svg.h>
#include <gnu_gama/local/test_linearization_visitor.h>
#include <sstream>
#include <cfloat>

// output double values with maximum precision available
// in QString::arg(double a DOUBLE_PRECISION)
#define DOUBLE_PRECISION , 0, 'g', DBL_MANT_DIG


Adjustment::Adjustment() : lsvg(nullptr), lnet(nullptr), solved(false)
{
}

Adjustment::~Adjustment()
{
  delete lsvg;
  delete lnet;
}

void Adjustment::read_configuration(QSqlQuery& q, const QString& configuration)
{

  struct Index {
    enum {
      conf_id, algorithm, sigma_apr, conf_pr, tol_abs, sigma_act,
      axes_xy, angles, epoch, ang_units, latitude, ellipsoid, cov_band
    };
  };

  q.exec("select conf_id, algorithm, "
         "       sigma_apr, conf_pr, tol_abs, sigma_act,"
         "       axes_xy, angles, epoch, ang_units, "
         "       latitude, ellipsoid, cov_band "
         "  from gnu_gama_local_configurations "
         " where conf_name = '" + configuration + "'");
  dberr(q, "DB read local_configurations");

  conf_id.clear();
  if (q.next())
    {
      conf_id = q.value(Index::conf_id).toString();
      conf_name = configuration;
    }
  else
    {
      QString conferr = tr("Configuration %1 not found");
      throw QGama::Exception(adjustmentException_,
                             conferr.arg(configuration));
    }

  delete lnet;
  lnet = new GNU_gama::local::LocalNetwork;

  const std::string alg = q.value(Index::algorithm).toString().toStdString();
  if (alg == "svd")
    lnet->set_algorithm("svd");
  else if (alg == "gso")
    lnet->set_algorithm("gso");
  else if (alg == "cholesky")
    lnet->set_algorithm("cholesky");
  else if (alg == "envelope")
    lnet->set_algorithm("envelope");
  else
    lnet->set_algorithm();

  lnet->apriori_m_0(q.value(Index::sigma_apr).toDouble());
  lnet->conf_pr    (q.value(Index::conf_pr).toDouble());
  lnet->tol_abs    (q.value(Index::tol_abs).toDouble());

  if (q.value(Index::sigma_act).toString() == "aposteriori")
    lnet->set_m_0_aposteriori();
  else
    lnet->set_m_0_apriori();

  {
    using namespace GNU_gama::local;

    LocalCoordinateSystem::CS lcs { LocalCoordinateSystem::CS::EN };
    QString s = q.value(Index::axes_xy).toString();
    if      (s == "en") lcs = LocalCoordinateSystem::CS::EN;
    else if (s == "nw") lcs = LocalCoordinateSystem::CS::NW;
    else if (s == "se") lcs = LocalCoordinateSystem::CS::SE;
    else if (s == "ws") lcs = LocalCoordinateSystem::CS::WS;
    else if (s == "ne") lcs = LocalCoordinateSystem::CS::NE;
    else if (s == "sw") lcs = LocalCoordinateSystem::CS::SW;
    else if (s == "es") lcs = LocalCoordinateSystem::CS::ES;
    else if (s == "wn") lcs = LocalCoordinateSystem::CS::WN;

    lnet->PD.local_coordinate_system = lcs;

    bool rang = (q.value(Index::angles).toString() == "right-handed");
    if (rang) lnet->PD.setAngularObservations_Righthanded();
    else      lnet->PD.setAngularObservations_Lefthanded();
  }

  lnet->set_epoch(q.value(Index::epoch).toDouble());

  if (q.value(Index::ang_units).toInt() == 400)
    lnet->set_gons();
  else
    lnet->set_degrees();

  // the following two attributes are not used in qgama --------------
  latitude = q.value(Index::latitude).toDouble()*M_PI/(lnet->gons() ? 200 : 180);
  ellipsoid.clear();
  if (!q.value(Index::ellipsoid).isNull())  ellipsoid = q.value(Index::ellipsoid).toString();
  // -----------------------------------------------------------------

  if (q.value(Index::cov_band).isNull())
    lnet->set_adj_covband();
  else
    lnet->set_adj_covband(q.value(Index::cov_band).toInt());

  description.clear();
  q.exec("select text from gnu_gama_local_descriptions "
         " where conf_id = '" + conf_id + "'"
                                          " order by indx asc");
  dberr(q, "DB read local_description");

  while (q.next())
    {
      description += q.value(0).toString();
    }

  lnet->description = description.toUtf8().data();

  fetch_points(q);
  fetch_clusters(q);

  delete lsvg;
  lsvg = new GNU_gama::local::GamaLocalSVG(lnet);

  solved = false;
  emit adjustment_signal(solved);
}


void Adjustment::fetch_points(QSqlQuery& q)
{
  q.exec("select id, x, y, z, txy, tz "
         "  from gnu_gama_local_points where conf_id = '" + conf_id + "'");
  dberr(q, "DB read local_points");

  while (q.next())
    {
      GNU_gama::local::LocalPoint p;

      if (!q.value(1).isNull() && !q.value(2).isNull())
        p.set_xy(q.value(1).toDouble(), q.value(2).toDouble());
      if (!q.value(3).isNull())
        p.set_z(q.value(3).toDouble());
      if (!q.value(4).isNull())
        {
          if      (q.value(4).toString() == "fixed")       p.set_fixed_xy();
          else if (q.value(4).toString() == "adjusted")    p.set_free_xy();
          else if (q.value(4).toString() == "constrained") p.set_constrained_xy();
        }
      if (!q.value(5).isNull())
        {
          if      (q.value(5).toString() == "fixed")       p.set_fixed_z();
          else if (q.value(5).toString() == "adjusted")    p.set_free_z();
          else if (q.value(5).toString() == "constrained") p.set_constrained_z();
        }

      const std::string pid = utf8_(q.value(0).toString());
      lnet->PD[pid] = p;
    }
}

std::string Adjustment::utf8_(const QString &s) const
{
  return std::string(s.toUtf8().data());
}

void Adjustment::fetch_clusters(QSqlQuery& q)
{
  q.exec("select ccluster, dim, band, tag "
         "  from gnu_gama_local_clusters where conf_id = '" + conf_id + "'");
  dberr(q, "DB read local_clusters");

  QSqlQuery r = q;
  while (q.next())
    {
      GNU_gama::Cluster<GNU_gama::local::Observation>* c = nullptr;

      QString cluster = q.value(0).toString();
      int     dim     = q.value(1).toInt();
      int     band    = q.value(2).toInt();
      QString tag     = q.value(3).toString();
      if (tag == "obs")
        {
          GNU_gama::local::StandPoint* p = new GNU_gama::local::StandPoint(&lnet->OD);
          fetch_obs(r, cluster, p);    // defines attribute StandPoint::station
          c = p;
        }
      else if (tag == "vectors")
        {
          GNU_gama::local::Vectors* p = new GNU_gama::local::Vectors(&lnet->OD);
          fetch_vectors(r, cluster, p);
          c = p;
        }
      else if (tag == "coordinates")
        {
          GNU_gama::local::Coordinates* p = new GNU_gama::local::Coordinates(&lnet->OD);
          fetch_coordinates(r, cluster, p);
          c = p;
        }
      else if (tag == "height-differences")
        {
          GNU_gama::local::HeightDifferences* p = new GNU_gama::local::HeightDifferences(&lnet->OD);
          fetch_height_differences(r, cluster, p);
          c = p;
        }
      lnet->OD.clusters.push_back(c);

      QSqlQuery p = q;
      c->covariance_matrix.reset(dim, band);
      c->covariance_matrix.set_zero();
      p.exec("select rind, cind, val "
             "  from gnu_gama_local_covmat "
             " where conf_id   = '" + conf_id + "'"
                                                "   and ccluster = " + cluster );
      dberr(p, "DB read local_covmat");

      while (p.next())
        {
          int    i = p.value(0).toInt();
          int    j = p.value(1).toInt();
          double d = p.value(2).toDouble();
          c->covariance_matrix(i,j) = d;
        }
      c->update();
    }
}


void Adjustment::fetch_obs(QSqlQuery& q, QString cluster, GNU_gama::local::StandPoint* c)
{
  // StandPoint cluster is a placeholder for "orientation shift" adjustment parameter, which
  // transforms directions (bearings) to "absolute bearings". All directions in the given
  // StandPoint cluster must have the same 'from' point. Other observation types in this
  // cluster may have different 'from' points.

  std::string dir_from;

  // const int icluster = cluster.toInt();
  q.exec("select indx, tag, from_id, to_id, to_id2, val, stdev, "
         "       from_dh, to_dh, to_dh2, dist, rejected"
         "  from gnu_gama_local_obs "
         " where conf_id  = '" + conf_id + "'"
                                           "   and ccluster = " + cluster );
  dberr(q, "DB read local_obs");

  while (q.next())
    {
      // unused :int indx = q.value( 0).toInt();
      QString     tag  = q.value( 1).toString();
      std::string from = utf8_(q.value( 2).toString());
      std::string to   = utf8_(q.value( 3).toString());
      std::string to2  = utf8_(q.value( 4).toString());
      double      val  = q.value( 5).toDouble();
      int     rejected = q.value(11).toInt();

      // optional attributes
      // double /*stdev,*/ from_dh, to_dh, to_dh2, dist;
      // bool   null_stdev   = q.value( 6).isNull();
      bool   null_from_dh = q.value( 7).isNull();
      bool   null_to_dh   = q.value( 8).isNull();
      bool   null_to_dh2  = q.value( 9).isNull();
      bool   null_dist    = q.value(10).isNull();

      GNU_gama::local::Observation* p = nullptr;
      GNU_gama::local::Angle* a = nullptr;

      if (tag == "direction")
        {
          p = new GNU_gama::local::Direction(from, to, val);
          if (dir_from.empty())
            {
              c->station = dir_from = from;
            }
          if (dir_from != from)
            {
              throw QGama::Exception(adjustmentException_,
                                     tr("StandPoint cluster with multiple directions sets"));
            }
        }
      else if (tag == "distance")   p = new GNU_gama::local::Distance  (from, to, val);
      else if (tag == "angle")      p = a = new GNU_gama::local::Angle (from, to, to2, val);
      else if (tag == "azimuth")    p = new GNU_gama::local::Azimuth   (from, to, val);
      else if (tag == "s-distance") p = new GNU_gama::local::S_Distance(from, to, val);
      else if (tag == "z-angle")    p = new GNU_gama::local::Z_Angle   (from, to, val);
      else if (tag == "dh")         p = new GNU_gama::local::H_Diff    (from, to, val);

      if (p == nullptr) throw(QGama::Exception(adjustmentException_,
                                               tr("Undefined observation type name")));

      if (!null_from_dh)     p->set_from_dh( q.value( 7).toDouble() );
      if (!null_to_dh)       p->set_to_dh  ( q.value( 8).toDouble() );
      if (!null_to_dh2 && a) a->set_fs_dh  ( q.value( 9).toDouble() );
      if (!null_dist)        p->set_from_dh( q.value(10).toDouble() );

      c->observation_list.push_back(p);


      if (rejected) p->set_passive();
    }
}


void Adjustment::fetch_vectors(QSqlQuery& q, QString cluster, GNU_gama::local::Vectors* c)
{
  // const int icluster = cluster.toInt();
  q.exec("select indx, from_id, to_id, dx, dy, dz, rejected "
         //"       ,from_dh, to_dh "
         "  from gnu_gama_local_vectors "
         " where conf_id  = '" + conf_id + "'"
                                           "   and ccluster = " + cluster );
  dberr(q, "DB read local_vectors");

  while (q.next())
    {
      std::string from_id  = utf8_(q.value(1).toString());
      std::string to_id    = utf8_(q.value(2).toString());
      double      dx       = q.value(3).toDouble();
      double      dy       = q.value(4).toDouble();
      double      dz       = q.value(5).toDouble();
      int         rejected = q.value(6).toInt();

      // double  from_dh, to_dh;
      // bool    from_dh_null = q.value(7).isNull();
      // bool    to_dh_null   = q.value(8).isNull();
      // if (!from_dh_null) from_dh = q.value(7).toDouble();
      // if (!to_dh_null)   to_dh   = q.value(8).toDouble();

      GNU_gama::local::Xdiff* x = new GNU_gama::local::Xdiff(from_id, to_id, dx);
      GNU_gama::local::Ydiff* y = new GNU_gama::local::Ydiff(from_id, to_id, dy);
      GNU_gama::local::Zdiff* z = new GNU_gama::local::Zdiff(from_id, to_id, dz);
      c->observation_list.push_back(x);
      c->observation_list.push_back(y);
      c->observation_list.push_back(z);

      if(rejected)
        {
          x->set_passive();
          y->set_passive();
          z->set_passive();
        }
    }
}


void Adjustment::fetch_coordinates(QSqlQuery& q, QString cluster, GNU_gama::local::Coordinates* c)
{
  // const int icluster = cluster.toInt();
  q.exec("select indx, id, x, y, z, rejected "
         "  from gnu_gama_local_coordinates "
         " where conf_id  = '" + conf_id + "'"
                                           "   and ccluster = " + cluster );
  dberr(q, "DB read local_coordinates");

  while (q.next())
    {
      double x {}, y{}, z{};
      // int         indx     = q.value(0).toInt();
      std::string id       = utf8_(q.value(1).toString());
      bool        xnull    = q.value(2).isNull();   if (!xnull) x = q.value(2).toDouble();
      bool        ynull    = q.value(3).isNull();   if (!ynull) y = q.value(3).toDouble();
      bool        znull    = q.value(4).isNull();   if (!znull) z = q.value(4).toDouble();
      int         rejected = q.value(5).toInt();

      if (xnull != ynull) throw QGama::Exception(adjustmentException_,
                                                 tr("Observed coordinates x and y must be defined both or none"));

      if (!xnull && !ynull)
        {
          GNU_gama::local::Observation* px = new GNU_gama::local::X(id, x);
          GNU_gama::local::Observation* py = new GNU_gama::local::Y(id, y);
          c->observation_list.push_back(px);
          c->observation_list.push_back(py);
          if (rejected)
            {
              px->set_passive();
              py->set_passive();
            }
        }
      if (!znull)
        {
          GNU_gama::local::Observation* pz = new GNU_gama::local::Z(id, z);
          c->observation_list.push_back(pz);
          if (rejected)
            {
              pz->set_passive();  // ??? is one rejection tag enough, does rejection of xy imply also rejection of z ???
            }
        }
    }

}


void Adjustment::fetch_height_differences(QSqlQuery& q, QString cluster, GNU_gama::local::HeightDifferences* c)
{
  // const int icluster = cluster.toInt();
  q.exec("select indx, tag, from_id, to_id, val, rejected, dist"
         "  from gnu_gama_local_obs "
         " where conf_id  = '" + conf_id + "'"
                                           "   and ccluster = " + cluster );
  dberr(q, "DB read local_obs / height differences");

  while (q.next())
    {
      // int         indx = q.value(0).toInt();
      QString     tag  = q.value(1).toString();
      std::string from = utf8_(q.value(2).toString());
      std::string to   = utf8_(q.value(3).toString());
      double      val  = q.value(4).toDouble();
      int     rejected = q.value(5).toInt();

      double dist {};
      bool   null_dist = q.value(6).isNull();
      if (!null_dist) dist = q.value(6).toDouble();

      if (tag != "dh")
        {
          QString err = "Illegal tag %1 in a <height-differences> cluster";
          throw QGama::Exception(adjustmentException_, tr(err.arg(tag).toStdString().c_str()));
        }

      GNU_gama::local::H_Diff* p = new GNU_gama::local::H_Diff(from, to, val);
      if (!null_dist) p->set_dist(dist);

      c->observation_list.push_back(p);

      if (rejected) p->set_passive();
    }
}


void Adjustment::exec()
{
  try
  {
    lnet->remove_inconsistency();

    GNU_gama::local::Acord2 acord(lnet->PD, lnet->OD);
    auto points = acord.execute();

    if (points.first > 0 || points.second > 0)  // solved xy or z
      {
        emit acord_signal();
      }

    lnet->update_points();
    lnet->solve();

    lnet->clear_linearization_iterations();
    while (lnet->next_linearization_iterations() &&
           TestLinearization(lnet))
      {
        lnet->increment_linearization_iterations();
        lnet->refine_approx();
        lnet->solve();
      }

    solved = true;
    emit adjustment_signal(solved);
  }
  catch (const GNU_gama::Exception::base& e)
  {
    throw QGama::Exception(adjustmentException_, e.what());
  }
}


QString Adjustment::xml()
{
  exec();
  /*
    bool gons = lnet->gons();
    if (gons)
       lnet->set_gons();
    else
       lnet->set_degrees();
    */
  lnet->description = utf8_(description);

  std::ostringstream ostr;
  GNU_gama::LocalNetworkXML xml(lnet);
  xml.write(ostr);

  return QString::fromUtf8(ostr.str().c_str());
}


int Adjustment::points() const
{
  return adjid.size();
}

QString Adjustment::get_description() const
{
  return description;
}

double  Adjustment::get_latitude() const
{
  return latitude/(M_PI/(lnet->gons() ? 200 : 180));
}

QString Adjustment::get_ellipsoid() const
{
  return ellipsoid;
}

QString Adjustment::get_conf_id() const
{
  return conf_id;
}

QString Adjustment::get_conf_name() const
{
  return conf_name;
}

double  Adjustment::get_sigma_apr() const
{
  return lnet->apriori_m_0();
}

double  Adjustment::get_conf_pr() const
{
  return lnet->conf_pr();
}

double  Adjustment::get_tol_abs() const
{
  return lnet->tol_abs();
}

QString Adjustment::get_sigma_act () const
{
  return lnet->m_0_apriori() ? "apriori" : "aposteriori";
}

QString Adjustment::get_axes_xy() const
{
  using namespace GNU_gama::local;
  LocalCoordinateSystem::CS
      lcs = lnet->PD.local_coordinate_system;

  //EN= 1, NW= 2, SE= 4, WS=  8,
  //NE=16, SW=32, ES=64, WN=128,

  QString axes = "ne";
  switch (lcs)
    {
    case LocalCoordinateSystem::CS::EN : axes = "en"; break;
    case LocalCoordinateSystem::CS::NW : axes = "nw"; break;
    case LocalCoordinateSystem::CS::SE : axes = "se"; break;
    case LocalCoordinateSystem::CS::WS : axes = "ws"; break;
    case LocalCoordinateSystem::CS::NE : axes = "ne"; break;
    case LocalCoordinateSystem::CS::SW : axes = "sw"; break;
    case LocalCoordinateSystem::CS::ES : axes = "es"; break;
    case LocalCoordinateSystem::CS::WN : axes = "wn"; break;
    default: break;
    }

  return axes;
}

QString Adjustment::get_angles() const
{
  return lnet->PD.left_handed_angles() ? "left-handed" : "right-handed";
}

double  Adjustment::get_epoch() const
{
  return lnet->epoch();
}

QString Adjustment::get_algorithm () const
{
  return lnet->algorithm().c_str();
}

int     Adjustment::get_ang_units () const
{
  return lnet->gons() ? 400 : 360;
}

void Adjustment::get(int n, point& p)
{
  get(adjid[--n].c_str(), p);
}


void Adjustment::get(QString str, point& p)
{
  p.id = str;
  std::string s = utf8_(str);

  p.indx = pid2indx[std::pair<std::string,char>(s, 'X')];
  p.indy = pid2indx[std::pair<std::string,char>(s, 'Y')];
  p.indz = pid2indx[std::pair<std::string,char>(s, 'Z')];

  if (!p.indx) p.x0 = p.dx = p.x = p.sx = p.cx = 0;
  if (!p.indy) p.y0 = p.dy = p.y = p.sy = p.cy = 0;
  if (!p.indz) p.z0 = p.dz = p.z = p.sz = p.cz = 0;

  const GNU_gama::local::LocalPoint b = lnet->PD[s];
  const GNU_gama::local::Vec&       x = lnet->solve();
  const double                  ccoef = lnet->conf_int_coef();

  if (p.indx)
    {
      p.x0 = b.x_0();
      p.dx = x(p.indx)/1000;
      p.x  = p.x0 + p.dx;
      p.sx = lnet->unknown_stdev(p.indx);
      p.cx = p.sx*ccoef;
    }

  if (p.indy)
    {
      p.y0 = b.y_0()*y_sign();
      p.dy = x(p.indy)/1000*y_sign();
      p.y  = p.y0 + p.dy;
      p.sy = lnet->unknown_stdev(p.indy);
      p.cy = p.sy*ccoef;
    }

  if (p.indz)
    {
      p.z0 = b.z_0();
      p.dz = x(p.indz)/1000;
      p.z  = p.z0 + p.dz;
      p.sz = lnet->unknown_stdev(p.indz);
      p.cz = p.sz*ccoef;
    }
}


int Adjustment::orientations() const
{
  return rcluster.size();
}


// cc/scale() --> sexadecimal seconds
double Adjustment::scale() const
{
  return lnet->gons() ? 1.0 : 0.324;
}


// y coordinates have internaly changed its sign for inconsistent systems
int Adjustment::y_sign() const
{
  return lnet->y_sign();
}


void Adjustment::get(int n, orientation& r)
{
  //#const AdjustmentCluster<GNU_gama::local::StandPoint>* asp =
  //#     static_cast<const AdjustmentCluster<GNU_gama::local::StandPoint>*>(rcluster[--n]);
  const GNU_gama::local::StandPoint* asp
      = static_cast<GNU_gama::local::StandPoint*>(rcluster[--n]);

  r.id = asp->station.str().c_str();
  //#r.cluster_id = asp->cluster;
  r.indr = asp->index_orientation();
  r.r0 = asp->orientation()*y_sign()*R2G;
  while (r.r0 <  0 ) r.r0 += 400;
  while (r.r0 > 400) r.r0 -= 400;
  r.dr = lnet->solve()(r.indr)*y_sign()/10000 ;
  r.r  = r.r0 + r.dr;
  while (r.r <  0 ) r.r += 400;
  while (r.r > 400) r.r -= 400;
  r.sr = lnet->unknown_stdev(r.indr);
  r.cr = r.sr*lnet->conf_int_coef();
}


void Adjustment::dberr(const QSqlQuery& q, QString title)
{
  QSqlError e = q.lastError();
  if (e.isValid()) throw QGama::Exception(title, e.text());
}

std::string Adjustment::svg()
{
  if (lsvg == 0) return std::string();

  return lsvg->string();
}

GNU_gama::local::GamaLocalSVG* Adjustment::get_svg()
{
  return lsvg;
}

//GNU_gama::local::LocalNetwork* Adjustment::get_local_network()
//{
//    return lnet;
//}
