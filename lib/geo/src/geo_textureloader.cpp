////
//// Created by jlemein on 22-02-21.
////
//#include <geo_texture.h>
//#include <geo_textureloader.h>
//
//#include <fmt/format.h>
//
//#define STB_IMAGE_IMPLEMENTATION
//
//#include <memory>
//#include <spdlog/spdlog.h>
//#include <stb_image.h>
//#include <wsp_workspace.h>
//#include <boost/algorithm/string.hpp>
//
////Please include your own zlib-compatible API header before
////including `tinyexr.h` when you disable `TINYEXR_USE_MINIZ`
////#define TINYEXR_USE_MINIZ 0
////#include "zlib.h"
//#define TINYEXR_IMPLEMENTATION
//
//#include <tinyexr.h>
//
//using namespace lsw;
//using namespace lsw::geo;
//using namespace std;
//
//namespace {
//    std::vector<std::string> SUPPORTED_EXTENSIONS = {".bmp", ".hdr", ".jpg", ".jpeg", ".png", ".exr"};
//
//    std::unique_ptr<res::Resource<Texture>> loadExr(std::filesystem::path path) {
//        float *pixelData{nullptr}; // width * height * RGBA
//        int width{0};
//        int height{0};
//        const char *err = nullptr;
//
//        int ret = LoadEXR(&pixelData, &width, &height, path.c_str(), &err);
//
//        if (ret != TINYEXR_SUCCESS) {
//            auto errorMessage = fmt::format("Failed loading texture '{}'", path.c_str());
//            if (err) {
//                errorMessage += ": " + std::string(err);
//                FreeEXRErrorMessage(err); // release memory of error message.
//            }
//            spdlog::warn(errorMessage);
//            return nullptr;
//        }
//
//        int desiredChannels = 4; // RGBA
//        auto texture =
//                make_unique<res::Resource<Texture>>(
//                        Texture{.path = path, .width = width, .height = height, .channels = desiredChannels});
//
//        // number of pixels * channels * 8 bit
//        uint64_t sizeImageData = width * height * desiredChannels;
//        (*texture)->data = std::vector<uint8_t>(pixelData, pixelData + sizeImageData);
//
//        free(pixelData); // release memory of image data
//
//        return texture;
//    }
//
//    std::unique_ptr<res::Resource<Texture>> loadStbImage(std::filesystem::path path) {
//        int desiredChannels = 4;
//        int width, height, channels;
//        unsigned char *pixelData = stbi_load(path.c_str(), &width, &height, &channels, desiredChannels);
//
//        if (pixelData == nullptr) {
//            throw std::runtime_error(
//                    fmt::format("Cannot load texture from file '{}'. Are you sure the "
//                                "filename and extension are correct?",
//                                path.c_str()));
//        }
//
//        if (width * height == 0) {
//            stbi_image_free(pixelData);
//            throw std::runtime_error(
//                    fmt::format("Cannot load texture from file '{}': width or height is 0", path.c_str()));
//        }
//
//        auto textureResource =
//                make_unique<res::Resource<Texture>>(
//                        Texture{.path = path, .width = width, .height = height, .channels = desiredChannels});
//        auto &texture = *textureResource;
//
//        // number of pixels * channels * 8 bit
//        uint64_t sizeImageData = width * height * desiredChannels;
//        texture->data = std::vector<uint8_t>(pixelData, pixelData + sizeImageData);
//        stbi_image_free(pixelData);
//
//        return textureResource;
//    }
//}
//
////std::unique_ptr<res::IResource> TextureLoader::createResource(const std::string &name) {
////    auto path = std::filesystem::path(name);  // R(name);
////
////    if (!std::filesystem::exists(path)) {
////        throw std::runtime_error(fmt::format("Cannot load texture from file '{}': file does not exist", std::filesystem::absolute(path).string()));
////    }
////
////    auto extLowerCase = boost::algorithm::to_lower_copy(path.extension().string());
////    if (std::find(SUPPORTED_EXTENSIONS.begin(), SUPPORTED_EXTENSIONS.end(), extLowerCase) ==
////        SUPPORTED_EXTENSIONS.end()) {
////        auto message = fmt::format("{} extension not supported, when trying to load '{}'. Supported extensions: [{}]",
////                                   extLowerCase, path.string(),
////                                   fmt::join(SUPPORTED_EXTENSIONS, ", "));
////        throw std::runtime_error(message);
////    }
////
////    std::unique_ptr<res::Resource<Texture>> texture{nullptr};
////    if (extLowerCase == ".exr") {
////        texture = loadExr(path);
////    } else {
////        texture = loadStbImage(path);
////    }
////
////    if (texture) {
////        spdlog::log(spdlog::level::info, "Loaded texture {}, {} x {}, {} channels", path.c_str(), (*texture)->width,
////                    (*texture)->height,
////                    (*texture)->channels);
////    }
////
////    return texture;
////}