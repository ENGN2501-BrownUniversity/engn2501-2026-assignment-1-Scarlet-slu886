//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2026-01-26 17:42:17 taubin>
//------------------------------------------------------------------------
//
// Faces.cpp
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

#include <math.h>
#include "Faces.hpp"
  
Faces::Faces(const int nV, const vector<int>& coordIndex) {
    // define variance
    _coordIndex = coordIndex;
    _nV = nV;
    _nF = 0;

    //scan all the data and calculate the number of faces
    int maxVertexIndex = -1;
    int currentFaceStart = 0;
    bool isNewFace = true;

    for (int i = 0; i < _coordIndex.size(); ++i){
        int val = _coordIndex[i];
        if (val == -1){
            _cornerFace.push_back(-1);
        }else{
            _cornerFace.push_back(_nF);
            if (val > maxVertexIndex) maxVertexIndex = val;
        }

        //test faces
        if (isNewFace) {
            if (val != -1){
                _faceFirstCorner.push_back(i);
                _nF++;
                isNewFace = false;
            }
        }

        //if meet -1, means current face is finished and the next one is new face
        if (val == -1) {
            isNewFace= true;
        }

    }

//check the number of vertices
    if (maxVertexIndex >= _nV) {
        _nV= maxVertexIndex + 1;
    }

}

int Faces::getNumberOfVertices() const {
  // TODO
  return _nV;
}

int Faces::getNumberOfFaces() const {
  // TODO
  return _nF;
}

int Faces::getNumberOfCorners() const {
  // TODO
    return (int)_coordIndex.size();
}

int Faces::getFaceSize(const int iF) const {
  // if step out then return to 0
  if ( iF <0 || iF >= _nF ) return 0;

  int start = _faceFirstCorner[iF];
  int count = 0;

  //count from the start untill -1
  for (int i= start; i< _coordIndex.size(); ++i){
      if (_coordIndex[i]== -1) break;
      count++;
  }
  return count;
}

int Faces::getFaceFirstCorner(const int iF) const {
  // TODO
  if (iF < 0 || iF >= _nF) return -1;
  return _faceFirstCorner[iF];
}

int Faces::getFaceVertex(const int iF, const int j) const {
  // TODO
  if (iF < 0 || iF >= _nF) return -1;

  int start = _faceFirstCorner[iF];
  int index = start +j;
  if (index >= _coordIndex.size() || _coordIndex[index] == -1) return -1;
  return _coordIndex[index];


}

int Faces::getCornerFace(const int iC) const {
  // TODO
    if (iC < 0 || iC >= _cornerFace.size()) return -1;
    return _cornerFace[iC];
    }

int Faces::getNextCorner(const int iC) const {
        if (iC < 0 || iC >= _coordIndex.size()) return -1;
        if (_coordIndex[iC] == -1) return -1;
        if (iC + 1 < _coordIndex.size() && _coordIndex[iC + 1] == -1) {
            int currentFace = _cornerFace[iC];
            return _faceFirstCorner[currentFace];
        }else {
            return iC + 1;
        }

}




