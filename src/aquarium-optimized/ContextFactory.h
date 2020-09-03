//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef CONTEXTFACTORY_H
#define CONTEXTFACTORY_H

class Context;
enum BACKENDTYPE : short;

class ContextFactory {
public:
  ContextFactory();
  ~ContextFactory();
  Context *createContext(BACKENDTYPE backendType);

private:
  Context *mContext;
};

#endif  // CONTEXTFACTORY_H
