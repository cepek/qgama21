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

#include "shrinkbandwidth.h"

bool shrinkBandWidth(GNU_gama::CovMat<>& cov)
{
  int N = cov.rows();
  int B = cov.bandWidth();
  int M = 0;

  for (int r=1; r<N; r++)
      for (int c=std::min(N,r+B); c>=r; c--)
          if (bool(cov(r,c)))     // explicit conversion is correct here
          {
            int b = c-r;
            if (b >= M)
            {
                M = b;
                if (M == B) return false;
            }
          }

  GNU_gama::CovMat<> tmp(N, M);
  for (int r=1; r<=N; r++)
      for (int c=r; c<=std::min(N,r+M); c++)
          tmp(r,c) = cov(r,c);

  cov = tmp;

  return true;
}
