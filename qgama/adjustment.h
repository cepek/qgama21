/*
  GNU Gama Qt based GUI
  Copyright (C) 2013, 2016, 2020 Ales Cepek <cepek@gnu.org>

  This file is part of GNU Gama.

  GNU Gama is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  GNU Gama is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ADJUSTMENT_H_QGama_h_qgama_h_qgama_adjustment_api
#define ADJUSTMENT_H_QGama_h_qgama_h_qgama_adjustment_api

#include <QObject>
#include <QtSql>
#include <matvec/inderr.h>

namespace QGama {

  class Exception : public GNU_gama::Exception::base {
  public:
    Exception(QString title, QString text) : title_(title), text_(text) {}
    virtual ~Exception() throw() {}

    QString text()  const { return text_; }
    QString title() const { return title_; }

  protected:

    const QString title_;
    const QString text_;

    // clone(), raise(), what()
    // redeclared as protexted to hide it in QGama::Exception
    //
    // what() error message is needed for cases when std::exception or
    // GNU_gama::Exception::basse is caught instead of QGama::Exception

    virtual Exception* clone() const { return new Exception(title_, text_); }
    virtual void       raise() const { throw *this; }

    const char* what() const throw()
    {
      // this should never happen
      return "Unhandled QGama::Exception";
    }

  }; // class Exception

} // namespace QGama


namespace GNU_gama { namespace local {
    class LocalNetwork;
    class StandPoint;
    class Coordinates;
    class HeightDifferences;
    class Vectors;
    class GamaLocalSVG;
}}

class Adjustment : public QObject
{
  Q_OBJECT

public:

  // adjustment internal observation types
  //# enum { distance, direction, angle, h_diff, s_distance, z_angle,
  //#        xdiff, ydiff, zdiff, xcoord, ycoord, zcoord };

  struct point
  {
    QString id;
    // internal indexes used in adjustment; zero if not defined
    int     indx, indy, indz;
    // approximate coordinates
    double  x0, y0, z0;
    // corrections
    double  dx, dy, dz;
    // adjusted coordinates
    double  x,  y,  z;
    // standard deviations
    double  sx, sy, sz;
    // confidence intervals
    double  cx, cy, cz;
  };

  struct orientation
  {
    // standpoint id
    QString id;
    // cluster
    int     cluster_id;
    // internal index of the orientation unknown
    int     indr;
    // approximate value
    double  r0;
    // correction
    double  dr;
    // adjusted value
    double  r;
    // standard deviation
    double  sr;
    // confidence interval
    double  cr;
  };

  Adjustment();
  ~Adjustment();

  void read_configuration(QSqlQuery&, const QString& configuration);
  void exec();
  QString xml();

  int  points() const;
  void get(QString, point&);
  void get(int,     point&);

  int orientations() const;
  void get(int, orientation&);

  QString get_description() const;
  double  get_latitude   () const;
  QString get_ellipsoid  () const;
  QString get_conf_id    () const;
  QString get_conf_name  () const;
  double  get_sigma_apr  () const;
  double  get_conf_pr    () const;
  double  get_tol_abs    () const;
  QString get_sigma_act  () const;
  QString get_axes_xy    () const;
  QString get_angles     () const;
  double  get_epoch      () const;
  QString get_algorithm  () const;
  int     get_ang_units  () const;

  std::string svg();

  GNU_gama::local::GamaLocalSVG* get_svg();
  GNU_gama::local::LocalNetwork* get_local_network() { return lnet; }

signals:
  void adjustment_signal(bool solved);
  void acord_signal();

private:

  GNU_gama::local::GamaLocalSVG* lsvg;
  GNU_gama::local::LocalNetwork* lnet;
  QString conf_id;
  QString conf_name;
  QString description;
  bool    solved;
  double  latitude;
  QString ellipsoid;

  // adjusted points ID list
  std::vector<std::string> adjid;
  // mapping point_id to adjustment index
  std::map<std::pair<std::string, char>, int> pid2indx;
  // clusters with orientation unknown
  std::vector<void*> rcluster;

  void fetch_points(QSqlQuery&);
  void fetch_clusters(QSqlQuery&);
  void fetch_obs(QSqlQuery&, QString, GNU_gama::local::StandPoint*);
  void fetch_vectors(QSqlQuery&, QString, GNU_gama::local::Vectors*);
  void fetch_coordinates(QSqlQuery&, QString, GNU_gama::local::Coordinates*);
  void fetch_height_differences(QSqlQuery&, QString, GNU_gama::local::HeightDifferences*);

  // check sql query
  void dberr(const QSqlQuery&, QString title);

  // some gama-local helper functions wrapper
  double scale () const;
  int    y_sign() const;

  // common export function for DB and SQL string
  QSqlQuery* exportQuery;
  QString*   exportString;
  QString    exportConfName;
  std::string utf8_(const QString& s) const;

  const QString adjustmentException_ = tr("Exception in the Adjustment occured!");
};

#endif // ADJUSTMENT_H
