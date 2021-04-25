//
// Created by magnias on 23/04/2021.
//

#ifndef _COMPUTEMANAGER_H_
#define _COMPUTEMANAGER_H_

#include <core/graphics/move_pipeline.h>
#include "../../vks/vks_pipeline.h"
#include "../../vks/vks_device.h"
#include "../../vks/vks_swap_chain.h"

using namespace vks;

class ComputeManager
{
 public:
	ComputeManager(VksDevice& device)
	:_device(device)
	{

	}

	void init(RayMarchPipeline& pipeline, MovePipeline& movePipeline)
	{
		_raymarchpipeline = &pipeline;
		_movepipeline = &movePipeline;
	}

	void render(Camera camera)
	{
		VkCommandBuffer cb = _device.beginSingleTimeCommands();

		_raymarchpipeline->begin(cb, 0);
		_raymarchpipeline->updateBuffers(camera);
		_raymarchpipeline->end();

		_device.endSingleTimeCommands(cb);
	}

 private:
	VksDevice& _device;
	RayMarchPipeline* _raymarchpipeline = nullptr;
	MovePipeline* _movepipeline = nullptr;

	bool showcompute = false;
};

#endif //_COMPUTEMANAGER_H_
