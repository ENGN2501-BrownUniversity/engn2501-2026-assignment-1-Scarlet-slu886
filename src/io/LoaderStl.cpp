//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2026-01-26 17:42:17 taubin>
//------------------------------------------------------------------------
//
// LoaderStl.cpp
//
// Written by: <Sijia Wei>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <math.h>

#include "TokenizerFile.hpp"
#include "LoaderStl.hpp"
#include "StrException.hpp"

#include "wrl/Shape.hpp"
#include "wrl/Appearance.hpp"
#include "wrl/Material.hpp"
#include "wrl/IndexedFaceSet.hpp"

using namespace std;

// reference
// https://en.wikipedia.org/wiki/STL_(file_format)

const char* LoaderStl::_ext = "stl";

bool LoaderStl::load(const char* filename, SceneGraph& wrl) {
  bool success = false;

  // clear the scene graph
  wrl.clear();
  wrl.setUrl("");

  FILE* fp = (FILE*)0;
  fprintf(stdout, "Starting LoaderStl::load for %s\n", filename);
  try {
      int facetCount = 0;

    // open the file
    if(filename==(char*)0) throw new StrException("filename==null");

    //use binary mode, so that can read it
    fp = fopen(filename, "rb");
    if(fp==(FILE*)0) throw new StrException("fp==(FILE*)0");

    //calcutale the size of files
    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    rewind(fp);

    fprintf(stdout, "File Size = %ld bytes\n", fileSize);




      // TODO ...

      // create the scene graph structure :
      // 1) the SceneGraph should have a single Shape node a child
      Shape* shape = new Shape();
      wrl.addChild(shape);

      // 2) the Shape node should have an Appearance node in its appearance field
      Appearance* appearance = new Appearance();
      shape->setAppearance(appearance);

      // 3) the Appearance node should have a Material node in its material field
      Material* material = new Material();
      appearance->setMaterial(material);

      // 4) the Shape node should have an IndexedFaceSet node in its geometry node
      // from the IndexedFaceSet
      IndexedFaceSet* ifs = new IndexedFaceSet();
      shape->setGeometry(ifs);

      // 5) get references to the coordIndex, coord, and normal arrays
      vector<int>& coordIndex = ifs->getCoordIndex();
      vector<float>& coord = ifs->getCoord();
      vector<float>& normal = ifs->getNormal();

      // 6) set the normalPerVertex variable to false (i.e., normals per face)
      ifs->setNormalPerVertex(false);

      // the file should contain a list of triangles in the following format

      // facet normal ni nj nk
      //   outer loop
      //     vertex v1x v1y v1z
      //     vertex v2x v2y v2z
      //     vertex v3x v3y v3z
      //   endloop
      // endfacet


      //test ASCII or binary
      bool isBinary = false;
      unsigned int numTriangles = 0;
      if (fileSize >= 84) {
          char header[80];
          if (fread(header, 1, 80, fp) == 80) {
              fread(&numTriangles, 4, 1, fp);
              //calculate the size of file, must match
              // 80 bytes header + 4 bytes count + 50 bytes per triangle
              long headerSize = 80;
              long countSize = 4;
              long triangleSize = 50;
              long expectedSize = headerSize + countSize + (numTriangles * triangleSize);
              if (fileSize == expectedSize) {
                  isBinary = true;
              }
          }
      }

      // ==========================================
      // A: Binary reading
      // ==========================================
      if (isBinary) {
          fprintf(stdout, "Format Detected = BINARY (Matches size formula)\n");
          facetCount = (int)numTriangles;
          int vertexCount = 0;

          float n[3], v1[3], v2[3], v3[3];
          // attribute byte count, unused but must read
          unsigned short attr;

          // must get back to the beginning
          fseek(fp, 84, SEEK_SET);

          for (unsigned int i = 0; i < numTriangles; i++) {
              if(fread(n, 4, 3, fp) != 3) break;
              if(fread(v1, 4, 3, fp) != 3) break;
              if(fread(v2, 4, 3, fp) != 3) break;
              if(fread(v3, 4, 3, fp) != 3) break;
              fread(&attr, 2, 1, fp);

              normal.push_back(n[0]); normal.push_back(n[1]); normal.push_back(n[2]);

              coord.push_back(v1[0]); coord.push_back(v1[1]); coord.push_back(v1[2]);
              coord.push_back(v2[0]); coord.push_back(v2[1]); coord.push_back(v2[2]);
              coord.push_back(v3[0]); coord.push_back(v3[1]); coord.push_back(v3[2]);

              coordIndex.push_back(vertexCount);
              coordIndex.push_back(vertexCount+1);
              coordIndex.push_back(vertexCount+2);
              coordIndex.push_back(-1);
              vertexCount += 3;
          }
          success = true;
      }
      // ==========================================
      // B: ASCII reading
      // ==========================================
      else {
          fprintf(stdout, "Format Detected = ASCII (Safe-Block + Auto-Normal)\n");

          rewind(fp);
          TokenizerFile tkn(fp);

          int vertexCount = 0;

          while (tkn.get()) {
              string token = tkn;

              if (token == "facet") {
                  vector<float> tempCoords;

                  while (tkn.get()) {
                      string subToken = tkn;

                      if (subToken == "vertex") {
                          float x=0, y=0, z=0;
                          if(tkn.get()) x = (float)atof(string(tkn).c_str());
                          if(tkn.get()) y = (float)atof(string(tkn).c_str());
                          if(tkn.get()) z = (float)atof(string(tkn).c_str());
                          tempCoords.push_back(x);
                          tempCoords.push_back(y);
                          tempCoords.push_back(z);
                      }
                      else if (subToken == "endfacet" || subToken == "endsolid") {
                          break;
                      }
                  }

                  if (tempCoords.size() == 9) {
                      facetCount++;

                      // Cross Product
                      // get 3 vertices P1, P2, P3
                      float p1[3] = {tempCoords[0], tempCoords[1], tempCoords[2]};
                      float p2[3] = {tempCoords[3], tempCoords[4], tempCoords[5]};
                      float p3[3] = {tempCoords[6], tempCoords[7], tempCoords[8]};

                      // calculate two vectors U = P2 - P1, V = P3 - P1
                      float u[3] = {p2[0]-p1[0], p2[1]-p1[1], p2[2]-p1[2]};
                      float v[3] = {p3[0]-p1[0], p3[1]-p1[1], p3[2]-p1[2]};

                      // calculate cross Product, get N
                      float nx = u[1]*v[2] - u[2]*v[1];
                      float ny = u[2]*v[0] - u[0]*v[2];
                      float nz = u[0]*v[1] - u[1]*v[0];

                      // Normalize
                      float len = sqrt(nx*nx + ny*ny + nz*nz);
                      if (len > 0.000001f) {
                          nx /= len; ny /= len; nz /= len;
                      } else {
                          // if get it is too small, give a default value
                          nx = 0; ny = 0; nz = 1;
                      }



                      normal.push_back(nx);
                      normal.push_back(ny);
                      normal.push_back(nz);


                      for (size_t k = 0; k < 9; k++) coord.push_back(tempCoords[k]);

                      coordIndex.push_back(vertexCount);
                      coordIndex.push_back(vertexCount+1);
                      coordIndex.push_back(vertexCount+2);
                      coordIndex.push_back(-1);
                      vertexCount += 3;
                  }
              }
          }

          if (facetCount > 0) success = true;
      }

      fclose(fp);

      /*
      if (success)
          fprintf(stderr, "DEBUG: Load Finished. Total facets loaded: %d\n", facetCount);
      else
          fprintf(stderr, "WARNING: Load failed (0 facets read).\n");
*/

  } catch(StrException* e) {
      if(fp!=(FILE*)0) fclose(fp);
      fprintf(stderr,"CRITICAL ERROR | %s\n", e->what());
      delete e;
      success = false;
  }

  return success;
}


