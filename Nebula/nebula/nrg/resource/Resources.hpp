#pragma once

#include <nrg/resource/Resource.hpp>
#include <nscene/Scene.hpp>
#include <nvk/Buffer.hpp>
#include <nvk/Image.hpp>

namespace Nebula::nrg
{
    nrg_decl_resource(BufferResource, ResourceType::eStorageBuffer, nvk::Buffer, buffer);
    nrg_decl_resource(ImageResource,  ResourceType::eImage,         nvk::Image,  image);
    nrg_decl_resource(SceneResource,  ResourceType::eSceneData,     ns::Scene,   scene);
}