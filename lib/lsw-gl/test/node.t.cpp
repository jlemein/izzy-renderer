////
//// Created by jeffrey on 16-12-21.
////
//#include <gtest/gtest.h>
//#include <glrs_effectgraph.h>
//#include <memory>
//#include <geo_material.h>
//using namespace izz::gl;
//
//class NodeTest : public ::testing::Test {
// protected:
//};
//
//TEST_F(NodeTest, TestSingleFramebufferEffect) {
//  EffectGraph bloomGraph;
//
//  Node extractBrightness;
//  extractBrightness.material = std::make_shared<izz::gl::Material>();
//  extractBrightness.inputs.colorBuffers[0] = FramebufferFormat::RGBA_FLOAT32;
//  extractBrightness.inputs.depthBuffer = FramebufferFormat::UNUSED;
//  extractBrightness.outputs.colorBuffers[0] = FramebufferFormat::RGBA_FLOAT32;
//  extractBrightness.outputs.colorBuffers[1] = FramebufferFormat::RGBA_FLOAT32;
//  auto id0 = bloomGraph.addNode(extractBrightness);
//
//  Node gaussianBlur;
//  gaussianBlur.material = std::make_shared<izz::gl::Material>();
//  gaussianBlur.inputs.colorBuffers[0] = FramebufferFormat::RGBA_FLOAT32;
//  gaussianBlur.outputs.colorBuffers[0] = FramebufferFormat::RGBA_FLOAT32;
//
//  auto id1 = bloomGraph.addNode(gaussianBlur);
//  auto id2 = bloomGraph.addNode(gaussianBlur);
//
//  Node combine;
//  combine.material = std::make_shared<izz::gl::Material>();
//  combine.inputs.colorBuffers[0] = FramebufferFormat::RGBA_FLOAT32;
//  combine.inputs.colorBuffers[1] = FramebufferFormat::RGBA_FLOAT32;
//  combine.outputs.colorBuffers[0] = FramebufferFormat::RGBA_UINT8;
//
//  auto id3 = bloomGraph.addNode(combine);
//  bloomGraph.addEdge(Edge{id0, id1});
//  bloomGraph.addEdge(Edge{id1, id2});
//  bloomGraph.addEdge(Edge{id0, id3});
//  bloomGraph.addEdge(Edge{id2, id3});
//
//  EXPECT_EQ(bloomGraph.)
//}