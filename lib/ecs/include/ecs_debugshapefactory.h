//
// Created by jlemein on 01-12-20.
//

#ifndef GLVIEWER_ECS_DEBUGSHAPEFACTORY_H
#define GLVIEWER_ECS_DEBUGSHAPEFACTORY_H

class DebugShapeFactory {
public:
  static shp::Mesh makeBoundingBox() {
    //    PrimitiveFactory
  }

  static shp::Mesh makeEulerArrow() {
    //    PrimitiveFactory
  }

  static shp::Mesh makeShapeFromDebugMode(DebugShape shape) {
    switch (shape) {
    case DebugShape::kBox:
      return makeBoundingBox();

    case DebugShape::kEulerArrow:
      return makeEulerArrow();

    default:
      throw std::runtime_error("Unsupported debug shape");
    }
  }
};
#endif // GLVIEWER_ECS_DEBUGSHAPEFACTORY_H
