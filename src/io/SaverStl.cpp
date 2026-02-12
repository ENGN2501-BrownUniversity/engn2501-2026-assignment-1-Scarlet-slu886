//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2026-01-26 17:42:17 taubin>
//------------------------------------------------------------------------
//
// SaverStl.cpp
//
// Written by: <Your Name>
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2026, Gabriel Taubin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Brown University nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL GABRIEL TAUBIN BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "SaverStl.hpp"

#include "wrl/Shape.hpp"
#include "wrl/Appearance.hpp"
#include "wrl/Material.hpp"
#include "wrl/IndexedFaceSet.hpp"
#include "core/Faces.hpp"
#include <cmath>
#include <vector>
#include "wrl/Node.hpp"
#include "wrl/Group.hpp"

using namespace std;

const char* SaverStl::_ext = "stl";

//////////////////////////////////////////////////////////////////////
bool SaverStl::save(const char* filename, SceneGraph& wrl) const {
  bool success = false;
  if(filename!=(char*)0) {

    // Check these conditions

    // 1) the SceneGraph should have a single child
    vector<Node*>& children = wrl.getChildren();
    //check whether it has a single child
    if (children.size() != 1) {
        return false;
    }


    // 2) the child should be a Shape node
    Node* child = children[0];
    Shape* shape = dynamic_cast<Shape*>(child);
    if (shape ==(Shape*)0) return false;


    // 3) the geometry of the Shape node should be an IndexedFaceSet node
    Node* geometry = shape->getGeometry();
    IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(geometry);
    if (ifs == (IndexedFaceSet*)0) return false;


    // - construct an instance of the Faces class from the IndexedFaceSet
    //we need first get the data and then construct faces
    int nV = ifs->getNumberOfCoord();
    const vector<int>& coordIndex = ifs->getCoordIndex();
    const vector<float>& coord = ifs->getCoord();

    //construct faces
    Faces* faces = new Faces(nV, coordIndex);

    // - remember to delete it when you are done with it (if necessary)
    //   before returning

    // 4) the IndexedFaceSet should be a triangle mesh
    // 5) the IndexedFaceSet should have normals per face

    // if (all the conditions are satisfied) {

    FILE* fp = fopen(filename,"w");
    if(	fp!=(FILE*)0) {

      // if set, use ifs->getName()
      // otherwise use filename,
      // but first remove directory and extension

      fprintf(fp,"solid %s\n",filename);

      // TODO ...
      // for each face {
      for (int iF = 0; iF < faces->getNumberOfFaces(); ++iF) {
          //get the number of vertices of faces
          int nCorners = faces->getFaceSize(iF);
          if (nCorners < 3) continue;

          //process the geometry data
          //garuantee it must be triangle
          int v0 = faces->getFaceVertex(iF, 0);
          float p0[3] = { coord[3*v0], coord[3*v0+1], coord[3*v0+2] };

          for (int k = 2; k < nCorners; ++k){
              int v1 = faces->getFaceVertex(iF, k-1);
              int v2 = faces->getFaceVertex(iF, k);

              float p1[3] = { coord[3*v1], coord[3*v1+1], coord[3*v1+2] };
              float p2[3] = { coord[3*v2], coord[3*v2+1], coord[3*v2+2] };

              float u[3] = { p1[0]-p0[0], p1[1]-p0[1], p1[2]-p0[2] };
              float v[3] = { p2[0]-p0[0], p2[1]-p0[1], p2[2]-p0[2] };

              float nx = u[1]*v[2] - u[2]*v[1];
              float ny = u[2]*v[0] - u[0]*v[2];
              float nz = u[0]*v[1] - u[1]*v[0];

              float len = sqrt(nx*nx + ny*ny + nz*nz);
              if (len > 0) { nx/=len; ny/=len; nz/=len; }

              fprintf(fp, "  facet normal %f %f %f\n", nx, ny, nz);
              fprintf(fp, "    outer loop\n");
              fprintf(fp, "      vertex %f %f %f\n", p0[0], p0[1], p0[2]);
              fprintf(fp, "      vertex %f %f %f\n", p1[0], p1[1], p1[2]);
              fprintf(fp, "      vertex %f %f %f\n", p2[0], p2[1], p2[2]);
              fprintf(fp, "    endloop\n");
              fprintf(fp, "  endfacet\n");

          }

      }

      //   ...
      // }
      fprintf(fp, "endsolid %s\n", filename);
      fclose(fp);
      success = true;
    }

    // } endif (all the conditions are satisfied)

  }
  return success;
}
