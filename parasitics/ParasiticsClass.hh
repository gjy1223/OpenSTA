// OpenSTA, Static Timing Analyzer
// Copyright (c) 2018, Parallax Software, Inc.
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef STA_PARASITICS_CLASS_H
#define STA_PARASITICS_CLASS_H

namespace sta {

class Parasitics;
class Parasitic;
class ParasiticDevice;
class ParasiticNode;
class ParasiticAnalysisPt;

typedef enum {
  reduce_parasitics_to_pi_elmore,
  reduce_parasitics_to_pi_pole_residue2,
  reduce_parasitics_to_none
} ReduceParasiticsTo;

} // namespace
#endif
