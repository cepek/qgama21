/*
  GNU Gama Qt based GUI
  Copyright (C) 2013 Ales Cepek <cepek@gnu.org>

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

#ifndef gama_local_xml2txt_XML2TXT_H
#define gama_local_xml2txt_XML2TXT_H

#include <iostream>
#include <gnu_gama/xml/localnetwork_adjustment_results.h>

namespace GNU_gama
{
   void xml2txt_general_parameters   (std::ostream& cout, const GNU_gama::LocalNetworkAdjustmentResults& adj);
   void xml2txt_adjusted_parameters  (std::ostream& cout, const GNU_gama::LocalNetworkAdjustmentResults& adj);
   void xml2txt_adjusted_observations(std::ostream& cout, const GNU_gama::LocalNetworkAdjustmentResults& adj);
}

#endif // XML2TXT_H
